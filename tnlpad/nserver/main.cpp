#define DEBUG
#include <iostream>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <errno.h>

#include "globals.h"
#include "channel.h"
#include "client.h"

#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include <signal.h>

#include "packet_stream.h"

using namespace std;

#define HDR_LINE					0x01
#define HDR_LINE_LEN				29
#define HDR_MESSAGE				0x02
#define HDR_DISCONNECT			0x04
#define HDR_CHANGE_CHANNEL		0x05
#define HDR_CLEAN_UP				0x06
#define HDR_LOG_NICK				0x08
#define HDR_LOG_NICK_LEN		16
#define HDR_NICK					0x09
#define HDR_NICK_LEN				16
#define HDR_BUCKET_FILL			0x0B
#define HDR_RECTANGLE			0x0A
#define HDR_ACTION				0x0C
#define HDR_LOG_CHANNEL			0x0D
#define HDR_LOG_CHANNEL_LEN	16
#define HDR_TOPIC					0x0E
#define HDR_LOG_CHANNEL_LIST	0x0F
#define HDR_LOGGED_IN			0x10
#define HDR_LOG_NAME				0x11
#define HDR_LOG_WEBSITE			0x12
#define HDR_LOG_EMAIL			0x13
#define HDR_LOGIN					0x14
#define HDR_SET_PASS				0x15
#define HDR_DROP_OPS				0x16
#define HDR_LOCK_CHAN			0x17
#define HDR_LOCK_TOPIC			0x18
#define HDR_TAKE_INK				0x19
#define HDR_TAKE_VOICE			0x1A
#define HDR_GIVE_INK				0x1B
#define HDR_GIVE_VOICE			0x1D
#define HDR_CIRCLE				0x1C
#define RESERVED1					0x1F
#define RESERVER2					0x20

void signal_catcher(int signal_caught)
{
	if(signal_caught == SIGPIPE)
	{
		cerr << "Caught signal SIGPIPE; Looks like a write to a closed socket" << endl;
	}
	return;
}

int listen_sock;
fd_set *client_socks;

static bool send_packet(Client *client, packet_stream *packet)
{
	errno = 0;
	send(client->get_id(),packet->get_data(),packet->get_length(),0);
	if(errno == 0)
		return true;
	else
	{
		// Send to a dead socket, lets delete it (maybe this is timeout?)
		client->set_delete();
		return false;
	}
}

static bool send_packet(Client *client, packet_stream *packet, long len)
{
	errno = 0;
	send(client->get_id(),packet->get_data(),len,0);
	if(errno == 0)
		return true;
	else 
	{
		// Send to a dead socket, lets delete it (maybe this is timeout?)
		client->set_delete();
		return false;
	}
}


static packet_stream *create_name_packet(char id, char *name)
{
	if(name == NULL)
		return NULL;
	packet_stream *name_packet = new packet_stream(34);
	*name_packet << '\x11' << id << name;
	return name_packet;
}

static packet_stream *create_website_packet(char id, char *website)
{
	if(website == NULL)
		return NULL;
	packet_stream *website_packet = new packet_stream(34);
	*website_packet << '\x12' << id << website;
	return website_packet;
}

static packet_stream *create_email_packet(char id, char *email)
{
	if(email == NULL)
		return NULL;
	packet_stream *email_packet = new packet_stream(34);
	*email_packet << '\x13' << id << email;	
	return email_packet;
}

// For sending to the client who  got their status changed
static void send_status_packet(Client *client, char id)
{
	packet_stream status(3);
	status << '\x1F' << id << (char)client->get_attribs();
	send_packet(client, &status, 3);
}

// For sending to clients to notify them that other users have lost priveledges
static void send_status_packet(Client *to, Client *client, char by_whom)
{
	packet_stream status(4);
	status << '\x20' << (char)client->get_id() << by_whom <<(char)client->get_attribs();
	send_packet(to, &status, 4);
}


static void socket_set_nonblock(int sock)
{
	int opts = fcntl(sock,F_GETFL,0);
   opts |= O_NONBLOCK;
   if(fcntl(sock,F_SETFL,opts)==0)
   {
      return;
   }
	else
	{
		cerr << "Failed setting nonblock on a socket, exitting" << endl;   
      exit(1);
      return;
   }
}

static char *change_nick(Client *client, char *nick)
{
	char *new_nick = new char[strlen(nick)+5];
	strcpy(new_nick, nick);
	for(vector<Client*>::iterator iter = clients.begin(); iter!=clients.end();iter++)
	{
		// Make sure there are no reoccuring nick names
		if(strcasecmp((*iter)->get_nick(),nick)==0)
		{
			strcpy(new_nick, nick);
			strcat(new_nick,"_");
			return change_nick(client, new_nick);
		}
	}
	client->set_nick(new_nick);
	return new_nick;
}

static void update_client_list(Client *client, Channel *channel)
{
	// This is called before the new client is added so it's all good
	//We need to tell existing clients of the new guy who's joined as well!
	packet_stream new_client_packet(18);
	new_client_packet << (char)HDR_LOG_CHANNEL_LIST << (char)client->get_id() << client->get_nick();

	packet_stream *new_client_name = create_name_packet((char)client->get_id(),client->get_name());
	packet_stream *new_client_website = create_website_packet((char)client->get_id(),client->get_website());
	packet_stream *new_client_email = create_email_packet((char)client->get_id(), client->get_email());
		
	for(vector<Client*>::iterator iter = channel->get_client_list()->begin();iter!=channel->get_client_list()->end();iter++)
	{
		// Create a packet of data that will update the joining client with 
		// the existing client list, and all of their details
		packet_stream packet(18);
		packet << (char)HDR_LOG_CHANNEL_LIST << (char)(*iter)->get_id() << (*iter)->get_nick();
		
		send_packet(client, &packet, 18);
		send_packet((*iter),&new_client_packet,18);
		
		//Create the data packets storing client details
		packet_stream *client_name = create_name_packet((char)(*iter)->get_id(),(*iter)->get_name());
		packet_stream *client_website = create_website_packet((char)(*iter)->get_id(),(*iter)->get_website());
		packet_stream *client_email = create_email_packet((char)(*iter)->get_id(), (*iter)->get_email());
		
		//Send new client existing client details
		if(client_name)		send_packet(client,client_name,34);
		if(client_website)	send_packet(client,client_website,34);
		if(client_email)		send_packet(client,client_email,34);
		
		//Send existing client details new client details
		if(new_client_name)			send_packet((*iter),new_client_name,34);
		if(new_client_website)		send_packet((*iter),new_client_website,34);
		if(new_client_email)			send_packet((*iter),new_client_email,34);
		send_status_packet((*iter), client, -1);
		send_status_packet(client, (*iter), -1);
	}
}

static void leave_channel(Client *client)
{
	if(client->get_channel()!=NULL)
	{
		// if the client is in another client, do the honourable thing, and leave it first!
		Channel *cur_channel = (Channel*)client->get_channel();
		cur_channel->remove_op(client);
		for(vector<Client*>::iterator iter = cur_channel->get_client_list()->begin(); iter!=cur_channel->get_client_list()->end();)
		{
			if(client != (*iter))
				iter++;
			else
			{
				cur_channel->get_client_list()->erase(iter);
				break;
			}
		}
	}
}

static void client_disconnect(Client *client)
{
	// When a client disconnects you need to tell the other clients so they
	// can update their list and so on.
	Channel *last_chan = (Channel*)client->get_channel();
	if(last_chan==NULL)
	{
		close(client->get_id());
		delete client;
		return;
	}
	//last_chan->remove_op(client);
	packet_stream packet(2);
	packet << '\x04' << (char)client->get_id();
	for(vector<Client*>::iterator iter = last_chan->get_client_list()->begin();iter != last_chan->get_client_list()->end();)
	{
		if((*iter)->get_id()!=client->get_id())
		{
			send_packet((*iter),&packet,2);
			iter++;
		}
		else
		{
			packet_stream self_disconnect(2);
			self_disconnect << '\x04' << (char)-1;
			send_packet(client,&self_disconnect,2);
			iter = last_chan->get_client_list()->erase(iter);
		}
	}
	close(client->get_id());
	delete client;
}

static void join_channel(Client *client, char *channel_name)
{
	bool found = false;
	if(channels.size()>0)
	{
		for(vector<Channel*>::iterator iter = channels.begin(); iter!=channels.end(); iter++)
		{
			if(strcmp(channel_name,(*iter)->get_name())==0)
			{
				//found the channel the user wanted to connect to!
				//Leave any other channel
				if(((*iter)->get_channel_attribs()&CHANNEL_LOCK)==CHANNEL_LOCK)
				{
					// Can't join the channel, it's locked out
					packet_stream locked_msg(64);
					locked_msg << '\x02' << (char)-1 << '\x02' << "Can't join " << (*iter)->get_name() << "; it is locked!" << '\x03';
					send_packet(client, &locked_msg);	
				}
				else
				{
					leave_channel(client);
					packet_stream welcome(18);
					welcome << '\x1E' << (char)-1 << (*iter)->get_name();
					send_packet(client,&welcome,18);
					client->set_attribs((*iter)->get_default_attribs());
					send_status_packet(client,-1);
					found = true;
					update_client_list(client,(*iter));
					(*iter)->get_client_list()->push_back(client);
					client->set_channel((void*)(*iter));
					if((*iter)->get_topic()!=NULL)
					{
						packet_stream topic_packet(132);
						topic_packet << (char)HDR_TOPIC << (char)-1 << '\x02' << (*iter)->get_topic() << '\x03';
						send_packet(client,&topic_packet);
					}
					//Now send a "Client Joined" to all the existing clients
					for(vector<Client*>::iterator client_it = (*iter)->get_client_list()->begin(); client_it!=(*iter)->get_client_list()->end();client_it++)
					{
						if((*client_it)!=client)
						{
							packet_stream join_packet(18);
							join_packet << '\x03' << (char)client->get_id() << client->get_nick();
							send_packet((*client_it),&join_packet,18);
						}
					}
				}
				if(!found)
				{
					join_channel(client,"#limbo");
				}
				return;
			}
		}
	}
	//if the channel wasn't found ... fall back to making it
	//TODO: set attributes of channel
	// No channel exists, so make it :D
	leave_channel(client);
	Channel *nchannel = new Channel(channel_name, channels.size()+1);
	packet_stream welcome(18);
	welcome << '\x1E' << (char)-1 << nchannel->get_name();
	send_packet(client, &welcome, 18);
	client->set_channel((void*)nchannel);
	client->set_attribs(nchannel->get_default_attribs());
	nchannel->get_client_list()->push_back(client);
	send_status_packet(client,-1);
	channels.push_back(nchannel);
}

int main(int argc, char **argv)
{
	cout << "NLPad Server V2.0 started..." << endl;
	
	signal(SIGPIPE,signal_catcher);
	
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	
	//We need to bind a port for our socket, so we need to make a sockaddr_in struct with info
	//on what we want it to act like
	
	struct sockaddr_in local_addr;
	
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(10101); // Host to network short - Network endian is BIGGGG
	local_addr.sin_addr.s_addr = INADDR_ANY;
	
	//bind these settings to the listening socket
	
	while(bind(listen_sock, (struct sockaddr*)&local_addr,sizeof(struct sockaddr))<0)
	{		
	}
	cout << "Listening for connections" << endl;
	//Now to listen
	
	listen(listen_sock, 255); //TWO HUNDRED AND FIFTY FIVE POSSIBLE CONNECTIONS!
	
	// Make limbo channel
	Channel *limbo = new Channel("#limbo",channels.size()+1);
	limbo->set_topic("Welcome to Limbo!");
	limbo->set_pass("socklops");
	limbo->set_channel_attribs(CHANNEL_TOPIC_LOCK);
	limbo->set_default_attribs(0); //No voice or ink!
	channels.push_back(limbo);
	
	client_socks = new fd_set;
	for(;;) // Loop until game over
	{
		int high_sock = 0;
		FD_ZERO(client_socks); // Clear the select list
		
		FD_SET(listen_sock, client_socks);
		high_sock = listen_sock + 1;
		if(clients.size()>0)
		{
			for(vector<Client*>::iterator iter = clients.begin(); iter!=clients.end();)
			{
				if((*iter)->get_delete())
				{
					client_disconnect((*iter));
					iter = clients.erase(iter);
					continue;
				}
					
				FD_SET((*iter)->get_id(),client_socks);
				if((*iter)->get_id()>=high_sock)
				{
					high_sock = (*iter)->get_id()+1;
				}
				iter++;
			}
		}
	
		int ret = select(high_sock, client_socks, NULL, NULL, NULL);
		if(ret==0)
			continue;
			
		if(FD_ISSET(listen_sock, client_socks))
		{
			// There is an incoming connection!! Answer it!
			Client *nclient = new Client();
			nclient->set_id(accept(listen_sock,NULL,NULL));
			clients.push_back(nclient);
			cout << "Got a new client " << nclient->get_id()  << endl;
		}
		
		for(vector<Client*>::iterator iter = clients.begin(); iter!=clients.end();)
		{
			if((*iter)->get_delete())
			{
				client_disconnect((*iter));
				iter = clients.erase(iter);
				continue;
			}
			Client *cur_it = (*iter);
			if(FD_ISSET(cur_it->get_id(), client_socks))
			{
				if(cur_it->get_logged_in())
				{
					// Not logged in, expecting name and some details
					char header = 0;
					recv(cur_it->get_id(),&header,1,0);
					if(header == HDR_LINE)
					{
						unsigned int x, y, x2, y2, r, g, b;
						char width;
						recv(cur_it->get_id(),&x,4,0);
						recv(cur_it->get_id(),&y,4,0);
						recv(cur_it->get_id(),&x2,4,0);
						recv(cur_it->get_id(),&y2,4,0);
						recv(cur_it->get_id(),&r,4,0);
						recv(cur_it->get_id(),&g,4,0);
						recv(cur_it->get_id(),&b,4,0);
						recv(cur_it->get_id(),&width,1,0);
						if((cur_it->get_attribs()&USER_INK)==USER_INK)
						{
							Channel *client_chan = (Channel*)cur_it->get_channel();
							packet_stream packet(31);
							packet << '\x01' <<(char)cur_it->get_id() << x << y << x2 << y2 << r << g << b << (char)width;
							for(vector<Client *>::iterator it = client_chan->get_client_list()->begin();it != client_chan->get_client_list()->end();it++)
							{
								if((*it)->get_id()==cur_it->get_id())
									continue;
								send_packet((*it),&packet);
							}
						}
					}
					
					if(header == HDR_MESSAGE)
					{
						packet_stream message_packet(258); // Messages can't be longer than 255 chars
						message_packet << (char)HDR_MESSAGE << (char)(*iter)->get_id() << '\x02';
						char buffer = 0;
						int counter = 0;
						while(buffer!='\x03')
						{
							if(counter>256)
							{
								cerr << "Received malformed message packet" << endl;
								client_disconnect((*iter));
								iter = clients.erase(iter);
								break;
							}
							recv((*iter)->get_id(),&buffer,1,0);
							message_packet << buffer;
							
							counter++;
						}
						if(counter>256) continue;
						
						if((cur_it->get_attribs()&USER_VOICE)==USER_VOICE)
						{
							// BUffer will now contain HDR_TOPIC Client ID \x02 data data data \x03
							Channel *client_chan = (Channel*)cur_it->get_channel();
							//TODO: Check the user has rights to talk (Voice?)
						
							for(vector<Client *>::iterator it = client_chan->get_client_list()->begin();it != client_chan->get_client_list()->end();it++)
							{
								if((*it)->get_id()==cur_it->get_id())
									continue;
								send_packet((*it),&message_packet);
							}
						}
					}
					if(header == HDR_CLEAN_UP)
					{
						Channel *client_chan = (Channel*)cur_it->get_channel();
						if((cur_it->get_attribs()&USER_INK)==USER_INK)
						{
							packet_stream packet(2);
							packet << '\x06' <<(char)cur_it->get_id();
							for(vector<Client *>::iterator it = client_chan->get_client_list()->begin();it != client_chan->get_client_list()->end();it++)
							{
								if((*it)->get_id()==cur_it->get_id())
									continue;
								send_packet((*it),&packet,2);
							}
						}
					}
					
					if(header == HDR_NICK)
					{
						char nick[HDR_LOG_NICK_LEN] = { '\0' };
						recv(cur_it->get_id(),nick, HDR_LOG_NICK_LEN,0);
						if(strcmp(change_nick((*iter),nick),nick)!=0)
						{
							packet_stream nick_changed(18);
							nick_changed << (char)HDR_NICK << (char)-1 << (*iter)->get_nick();
						
							send_packet((*iter), &nick_changed,18);
						}
						packet_stream new_nick(18);
						new_nick << (char)HDR_NICK << (char)cur_it->get_id() << (*iter)->get_nick();
							
						Channel *client_chan = (Channel*)cur_it->get_channel();
						for(vector<Client *>::iterator it = client_chan->get_client_list()->begin();it != client_chan->get_client_list()->end();it++)
						{
							if((*it)->get_id()==cur_it->get_id())
								continue;
							send_packet((*it),&new_nick,18);
						}
					}
					
					if(header == HDR_DISCONNECT)
					{
						client_disconnect((*iter));
						iter = clients.erase(iter);
						continue;
					}
					
					if(header == HDR_RECTANGLE)
					{
						unsigned int x, y, x2, y2, r, g, b;
						char width;
						recv(cur_it->get_id(),&x,4,0);
						recv(cur_it->get_id(),&y,4,0);
						recv(cur_it->get_id(),&x2,4,0);
						recv(cur_it->get_id(),&y2,4,0);
						recv(cur_it->get_id(),&r,4,0);
						recv(cur_it->get_id(),&g,4,0);
						recv(cur_it->get_id(),&b,4,0);
						recv(cur_it->get_id(),&width,1,0);
						if((cur_it->get_attribs()&USER_INK)==USER_INK)
						{
							Channel *client_chan = (Channel*)cur_it->get_channel();
							packet_stream packet(31);
							packet << '\x0A' <<(char)cur_it->get_id() << x << y << x2 << y2 << r << g << b << (char)width;
							for(vector<Client *>::iterator it = client_chan->get_client_list()->begin();it != client_chan->get_client_list()->end();it++)
							{
								if((*it)->get_id()==cur_it->get_id())
									continue;					
								send_packet((*it),&packet);
							}
						}
					}
					
					if(header == HDR_CIRCLE)
					{
						unsigned int x, y, x2, y2, r, g, b;
						char width;
						recv(cur_it->get_id(),&x,4,0);
						recv(cur_it->get_id(),&y,4,0);
						recv(cur_it->get_id(),&x2,4,0);
						recv(cur_it->get_id(),&y2,4,0);
						recv(cur_it->get_id(),&r,4,0);
						recv(cur_it->get_id(),&g,4,0);
						recv(cur_it->get_id(),&b,4,0);
						recv(cur_it->get_id(),&width,1,0);
						if((cur_it->get_attribs()&USER_INK)==USER_INK)
						{
							Channel *client_chan = (Channel*)cur_it->get_channel();
							packet_stream packet(31);
							packet << '\x1C' <<(char)cur_it->get_id() << x << y << x2 << y2 << r << g << b << (char)width;
							for(vector<Client *>::iterator it = client_chan->get_client_list()->begin();it != client_chan->get_client_list()->end();it++)
							{
								if((*it)->get_id()==cur_it->get_id())
									continue;					
								send_packet((*it),&packet);
							}
						}
					}
					
					if(header == HDR_BUCKET_FILL)
					{
						unsigned int x, y, r, g, b;
						recv(cur_it->get_id(),&x,4,0);
						recv(cur_it->get_id(),&y,4,0);
						recv(cur_it->get_id(),&r,4,0);
						recv(cur_it->get_id(),&g,4,0);
						recv(cur_it->get_id(),&b,4,0);
						if((cur_it->get_attribs()&USER_INK)==USER_INK)
						{
							Channel *client_chan = (Channel*)cur_it->get_channel();
							packet_stream packet(22);
							packet << '\x0B' <<(char)cur_it->get_id() << x << y << r << g << b;
							for(vector<Client *>::iterator it = client_chan->get_client_list()->begin();it != client_chan->get_client_list()->end();it++)
							{
								if((*it)->get_id()==cur_it->get_id())
									continue;
								send_packet((*it),&packet);
							}
						}
					}
					
					if(header == HDR_ACTION)
					{
						packet_stream message_packet(258); // Messages can't be longer than 255 chars
						message_packet << (char)HDR_ACTION << (char)(*iter)->get_id() << '\x02';
						char buffer = 0;
						int counter = 0;
						while(buffer!='\x03')
						{
							if(counter>256)
							{
								cerr << "Received malformed action packet" << endl;
								client_disconnect((*iter));
								iter = clients.erase(iter);
								break;
							}
							recv((*iter)->get_id(),&buffer,1,0);
							message_packet << buffer;
							
							counter++;
						}
						if(counter>256) continue;
						
						if((cur_it->get_attribs()&USER_VOICE)==USER_VOICE)
						{
							// BUffer will now contain HDR_TOPIC Client ID \x02 data data data \x03
							Channel *client_chan = (Channel*)cur_it->get_channel();
							//TODO: Check the user has rights to talk (Voice?)
						
							for(vector<Client *>::iterator it = client_chan->get_client_list()->begin();it != client_chan->get_client_list()->end();it++)
							{
								if((*it)->get_id()==cur_it->get_id())
									continue;
								send_packet((*it),&message_packet);
							}
						}
					}
					
					if(header == HDR_LOG_CHANNEL)
					{
						char channel[HDR_LOG_CHANNEL_LEN] = { '\0' };
						recv(cur_it->get_id(), channel, HDR_LOG_CHANNEL_LEN,0);
						packet_stream leave_channel_packet(2);
						leave_channel_packet << '\x05' << (char)cur_it->get_id();
						Channel *client_chan = (Channel*)cur_it->get_channel();
						for(vector<Client *>::iterator it = client_chan->get_client_list()->begin();it != client_chan->get_client_list()->end();it++)
						{
							if((*it)->get_id()==cur_it->get_id())
								continue;					
							send_packet((*it),&leave_channel_packet);
						}
						join_channel((*iter),channel);
					}
					if(header == HDR_TOPIC)
					{
						packet_stream topic_packet(132); // Topics can't be more than 128 character (+ 4 bytes for header, client id, begin text, end text)
						topic_packet << (char)HDR_TOPIC << (char)(*iter)->get_id() << '\x02';
						char buffer = 0;
						int counter = 0;
						char *topic = new char[128];
						memset(topic, '\0', 128);
						while(buffer!='\x03')
						{
							if(counter>129)
							{
								cerr << "Received malformed topic packet" << endl;
								client_disconnect((*iter));
								iter = clients.erase(iter);
								break;
							}
							recv((*iter)->get_id(),&buffer,1,0);
							topic_packet << buffer;
							if(buffer!='\03')
								topic[counter] = buffer;
							counter++;
						}
						if(counter>129) continue;
						
						// BUffer will now contain HDR_TOPIC Client ID \x02 data data data \x03
						Channel *client_chan = (Channel*)cur_it->get_channel();
						
						if((client_chan->get_channel_attribs()&CHANNEL_TOPIC_LOCK)==CHANNEL_TOPIC_LOCK && !client_chan->is_op(cur_it))
						{
							packet_stream failed(64);
							failed << '\x02' << (char)-1 << '\x02' << "Topic is locked and cannot be altered" << '\x03';
							send_packet(cur_it,&failed);
						}
						else
						{
							//TODO: Check the user has writes to set the topic (Voice?) // though this could be done client side
							client_chan->set_topic(topic);
							packet_stream success(64);
							success << '\x02' << (char)-1 << '\x02' << "You set the topic for " << client_chan->get_name() << '\x03';
							send_packet(cur_it,&success);
							for(vector<Client *>::iterator it = client_chan->get_client_list()->begin();it != client_chan->get_client_list()->end();it++)
							{
								if((*it)->get_id()==cur_it->get_id())
									continue;
								send_packet((*it),&topic_packet);
							}
						}
					}

					if(header == HDR_LOGIN)
					{
						/* Login takes 32 bytes for a password and if it's correct adds the user to the ops list in the channel */
						
						Channel *client_chan = (Channel*)cur_it->get_channel();
						char password_rec[33] = { '\0' };
						recv(cur_it->get_id(),password_rec,32,0);
						cout << "DEBUG: Got password for " << client_chan->get_name() << " " << password_rec << endl;
						if(strcmp(client_chan->get_pass(),password_rec)!=0)
						{
							cout << "DEBUG: Client " << cur_it->get_nick() << " failed logging in " << endl;
							packet_stream failed_message(64);
							failed_message << '\x02' << (char)-1 << '\x02' <<"Channel Authentication failed!" << '\x03';
							send_packet(cur_it,&failed_message);
						}
						else
						{
							client_chan->add_op(cur_it);
							cout << "DEBUG: Client " << cur_it->get_name() << " successfully logged in to " << client_chan->get_name() << endl;
							packet_stream packet(64);
							packet << '\x02' << (char)-1 << '\x02' << "You are now an operator of " << client_chan->get_name() << '\x03';
							send_packet(cur_it,&packet);
							packet.clear_data();
							packet << '\x14' << (char)-1;
							send_packet(cur_it,&packet);
							packet.clear_data();
							packet << '\x02' << (char)-1 << '\x02' << cur_it->get_nick() << " obtains operator status" << '\x03';
							for(vector<Client *>::iterator it = client_chan->get_client_list()->begin();it != client_chan->get_client_list()->end();it++)
							{
								if((*it)->get_id()==cur_it->get_id())
									continue;
								send_packet((*it),&packet);
							}
						}
					}
					if(header == HDR_SET_PASS)
					{
						/* Set pass takes 32 bytes for a password and if there is no password, or if the client is actually an op, it sets the channel's password */
						
						Channel *client_chan = (Channel*)cur_it->get_channel();
						char password_rec[33] = { '\0' };
						recv(cur_it->get_id(),password_rec,32,0);
						cout << "DEBUG: Got password for " << client_chan->get_name() << " " << password_rec << endl;
						if(client_chan->get_pass()[0] == '\0')
						{
							// No password
							cout << "Adding op" <<endl;
							client_chan->add_op(cur_it);
							client_chan->set_pass(password_rec);
							cout << "DEBUG: Client " << cur_it->get_name() << " successfully logged in to " << client_chan->get_name() << endl;
							packet_stream packet(64);
							packet << '\x02' << (char)-1 << '\x02' << "You are now an operator of " << client_chan->get_name() << '\x03';
							send_packet(cur_it, &packet);
							packet.clear_data();
							packet << '\x14' << (char)-1;
							send_packet(cur_it,&packet);
							packet.clear_data();
							packet << '\x02' << (char)-1 << '\x02' << cur_it->get_nick() << " obtains operator status" << '\x03';
							for(vector<Client *>::iterator it = client_chan->get_client_list()->begin();it != client_chan->get_client_list()->end();it++)
							{
								if((*it)->get_id()==cur_it->get_id())
									continue;
								send_packet((*it),&packet);
							}
						}
						else if(client_chan->is_op(cur_it))
						{
							cout << "DEBUG: Client " << cur_it->get_nick() << " is op and changing pass " << endl;
							packet_stream password_change(64);
							client_chan->set_pass(password_rec);
							password_change << '\x02' << (char)-1 << '\x02' << "Password for " << client_chan->get_name() << " changed!" << '\x03';
							send_packet(cur_it,&password_change);
						}
						else
						{
							//Client isn't an op, and the channel already has a password so, tell him where to go
							packet_stream register_failed(128);
							register_failed << '\x02' << (char)-1 << '\x02' << "The channel is already registered and you are not recognised as an op! Password couldn't be set." << '\x03';
							send_packet(cur_it,&register_failed);
						}
					}
					if(header == HDR_LOCK_CHAN)
					{
						/* Set pass takes 32 bytes for a password and if there is no password, or if the client is actually an op, it sets the channel's password */
						Channel *client_chan = (Channel*)cur_it->get_channel();
						packet_stream mass_message(64);
						mass_message << '\x02' << (char)-1;
						if(client_chan->is_op(cur_it))
						{
							if((client_chan->get_channel_attribs()&CHANNEL_LOCK)==CHANNEL_LOCK)
							{
								client_chan->set_channel_attribs(client_chan->get_channel_attribs()^CHANNEL_LOCK);
								packet_stream unlocked(32);
								unlocked << '\x02' << (char)-1 << '\x02' << "You unlocked the channel" << '\x03';
								send_packet(cur_it,&unlocked);
								mass_message << '\x02' << "Channel is now unlocked" << '\x03';
							}
							else
							{
								client_chan->set_channel_attribs(client_chan->get_channel_attribs()|CHANNEL_LOCK);
								packet_stream locked(32);
								locked << '\x02' << (char)-1 << '\x02' << "You locked the channel" << '\x03';
								send_packet(cur_it,&locked);
								mass_message << '\x02' << "Channel is now locked" << '\x03';
							}
							for(vector<Client *>::iterator it = client_chan->get_client_list()->begin();it != client_chan->get_client_list()->end();it++)
							{
								if((*it)->get_id()==cur_it->get_id())
									continue;
								send_packet((*it),&mass_message);
							}
						}
						else
						{
							packet_stream failed(64);
							failed << '\x02' << (char)-1 << '\x02' << "You need ops to lock a channel" << '\x03';
							send_packet(cur_it,&failed);
						}
					}
					if(header == HDR_LOCK_TOPIC)
					{
						/* Set pass takes 32 bytes for a password and if there is no password, or if the client is actually an op, it sets the channel's password */
						Channel *client_chan = (Channel*)cur_it->get_channel();
						packet_stream mass_message(64);
						mass_message << '\x02' << (char)-1;
						if(client_chan->is_op(cur_it))
						{
							if((client_chan->get_channel_attribs()&CHANNEL_TOPIC_LOCK)==CHANNEL_TOPIC_LOCK)
							{
								client_chan->set_channel_attribs(client_chan->get_channel_attribs()^CHANNEL_TOPIC_LOCK);
								packet_stream unlocked(32);
								unlocked << '\x02' << (char)-1 << '\x02' << "You unlocked the topic" << '\x03';
								send_packet(cur_it,&unlocked);
								mass_message << '\x02' << "Channel topic is now unlocked" << '\x03';
							}
							else
							{
								client_chan->set_channel_attribs(client_chan->get_channel_attribs()|CHANNEL_TOPIC_LOCK);
								packet_stream locked(32);
								locked << '\x02' << (char)-1 << '\x02' << "You locked the topic" << '\x03';
								send_packet(cur_it,&locked);
								mass_message << '\x02' << "Channel topic is now locked" << '\x03';
							}
							for(vector<Client *>::iterator it = client_chan->get_client_list()->begin();it != client_chan->get_client_list()->end();it++)
							{
								if((*it)->get_id()==cur_it->get_id())
									continue;
								send_packet((*it),&mass_message);
							}
						}
						else
						{
							packet_stream failed(64);
							failed << '\x02' << (char)-1 << '\x02' << "You need ops to lock the topic" << '\x03';
							send_packet(cur_it,&failed);
						}
					}
					/*
					#define HDR_TAKE_INK				0x19
					#define HDR_TAKE_VOICE			0x1A
					#define HDR_GIVE_INK				0x1B
					#define HDR_GIVE_VOICE			0x1D
					*/
					if(header == HDR_TAKE_INK || header == HDR_TAKE_VOICE || header == HDR_GIVE_INK || header == HDR_GIVE_VOICE )
					{
						
						char client_id;
						recv(cur_it->get_id(),&client_id,1,0);
						cout << "Attemping to modify client with id of " << (int)client_id << endl;
						Channel *client_chan = (Channel*)cur_it->get_channel();
						if(!client_chan->is_op(cur_it))
						{
							packet_stream failed(64);
							failed << '\x02' << (char)-1 << '\x02' << "You need ops to modify clients abilities" << '\x03';
							send_packet(cur_it,&failed);
							
						}
						else
						{
							//Now that the id of who we need to alter is here, lets do the dirty
					
							for(vector<Client*>::iterator client_it = client_chan->get_client_list()->begin(); client_it != client_chan->get_client_list()->end(); client_it++)
							{
								if((*client_it)->get_id()==client_id)
								{
									if(header==HDR_TAKE_INK)
									{
										if(((*client_it)->get_attribs()&USER_INK)==USER_INK)
										{
											(*client_it)->set_attribs((*client_it)->get_attribs()^USER_INK);
											send_status_packet((*client_it),cur_it->get_id());
											packet_stream success(64);
											success << '\x02' << (char)-1 << '\x02' << "You took ink from " << (*client_it)->get_nick() << '\x03';
											send_packet(cur_it,&success);
											for(vector<Client*>::iterator it = client_chan->get_client_list()->begin(); it != client_chan->get_client_list()->end(); it++)
											{
												if((*it)!=cur_it && (*it)!=(*client_it))
													send_status_packet((*it),(*client_it), cur_it->get_id());
											}
										}
										
									}
									if(header==HDR_TAKE_VOICE)
									{
										if(((*client_it)->get_attribs()&USER_VOICE)==USER_VOICE)
										{
											(*client_it)->set_attribs((*client_it)->get_attribs()^USER_VOICE);
											send_status_packet((*client_it),cur_it->get_id());
											packet_stream success(64);
											success << '\x02' << (char)-1 << '\x02' << "You took voice from " << (*client_it)->get_nick() << '\x03';
											send_packet(cur_it, &success);
											for(vector<Client*>::iterator it = client_chan->get_client_list()->begin(); it != client_chan->get_client_list()->end(); it++)
											{
												if((*it)!=cur_it && (*it)!=(*client_it))
													send_status_packet((*it),(*client_it), cur_it->get_id());
											}
										}
									}
									if(header==HDR_GIVE_INK)
									{
										if(((*client_it)->get_attribs()&USER_INK)==0)
										{
											(*client_it)->set_attribs((*client_it)->get_attribs()|USER_INK);
											send_status_packet((*client_it),cur_it->get_id());
											packet_stream success(64);
											success << '\x02' << (char)-1 << '\x02' << "You gave ink to " << (*client_it)->get_nick() << '\x03';
											send_packet(cur_it,&success);
											for(vector<Client*>::iterator it = client_chan->get_client_list()->begin(); it != client_chan->get_client_list()->end(); it++)
											{
												if((*it)!=cur_it && (*it)!=(*client_it))
													send_status_packet((*it),(*client_it), cur_it->get_id());
											}
										}
									}
									if(header==HDR_GIVE_VOICE)
									{
										if(((*client_it)->get_attribs()&USER_VOICE)==0)
										{
											(*client_it)->set_attribs((*client_it)->get_attribs()|USER_VOICE);
											send_status_packet((*client_it),cur_it->get_id());
											packet_stream success(64);
											success << '\x02' << (char)-1 << '\x02' << "You gave voice to " << (*client_it)->get_nick() << '\x03';
											send_packet(cur_it,&success);
											for(vector<Client*>::iterator it = client_chan->get_client_list()->begin(); it != client_chan->get_client_list()->end(); it++)
											{
												if((*it)!=cur_it && (*it)!=(*client_it))
													send_status_packet((*it),(*client_it), cur_it->get_id());
											}
										}
									}
								}
							}
						}
					}
				}
				else
				{
					// Not logged in, expecting nick and some details
					char header = 0;
					recv(cur_it->get_id(),&header,1,0);
					if(header == HDR_LOG_NICK)
					{
						char nick[HDR_LOG_NICK_LEN] = { '\0' };
						recv(cur_it->get_id(),nick, HDR_LOG_NICK_LEN,0);
						if(strcmp(change_nick((*iter),nick),nick)!=0)
						{
							packet_stream nick_changed(18);
							nick_changed << (char)HDR_NICK << (char)-1 << (*iter)->get_nick();
							
							send_packet((*iter), &nick_changed, 18);
						}
					} 
					else if(header == HDR_LOG_CHANNEL)
					{
						char channel[HDR_LOG_CHANNEL_LEN] = { '\0' };
						recv(cur_it->get_id(), channel, HDR_LOG_CHANNEL_LEN,0);
						join_channel(cur_it, channel);
					}
					else if(header == HDR_LOG_NAME)
					{
						char name[32] = { '\0' };
						recv(cur_it->get_id(), name, 32, 0);
						cur_it->set_name(name);
					}
					else if(header == HDR_LOG_WEBSITE)
					{
						char website[32] = { '\0' };
						recv(cur_it->get_id(), website, 32, 0);
						cur_it->set_website(website);
					}
					else if(header == HDR_LOG_EMAIL)
					{
						char email[32] = { '\0' };
						recv(cur_it->get_id(), email, 32, 0);
						cur_it->set_email(email);
					}
					else if(header == HDR_LOGGED_IN)
					{
						cur_it->set_logged_in();
					}
					else
					{
						close(cur_it->get_id());
						delete cur_it;
						iter = clients.erase(iter);
						continue;
					}
				}
			}
			if(errno == EPIPE)
			{
				client_disconnect((*iter));
				iter = clients.erase(iter);
				errno = 0;
				cout << "DEBUG: Removing a client after a EPIPE - BAD JUJU (could be someone we sent to..)" << endl;
				continue;
			}
			iter++;
		}
	}	
	return 0;
}
