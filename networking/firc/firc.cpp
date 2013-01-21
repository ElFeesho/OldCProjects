#include "firc.hpp"
#include <iostream>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

firc::firc(const char *server, short port):sock(0),server(server),port(port)
{
	sock = socket(AF_INET,SOCK_STREAM,0);
}

firc::~firc()
{
	if(on_close!=NULL)
		on_close(this);
	disconnect();	
}

void firc::set_raw_data(void (*on_raw_data_functor)(firc *instance, const char *data, int length))
{
	on_raw_data = on_raw_data_functor;
}

void firc::set_privmsg(void (*on_data_functor)(firc *instance, const char *nick, const char *channel, const char *message))
{
	on_privmsg = on_data_functor;
}

void firc::set_command(void (*on_command_functor)(firc *instance, const char *nick, const char *channel, const char *command, const char *args), const char *delim)
{
	command_delim = new char[1];
	memcpy((void*)command_delim,(void*)delim,1);
	on_command = on_command_functor;
}

void firc::set_close(void (*on_close_func)(firc *instance))
{
	on_close = on_close_func;
}

void firc::set_command_delim(const char *delim)
{
	delete command_delim;
	command_delim = new char[1];
	memcpy((void*)command_delim,(void*)delim,1);
}

void firc::disconnect()
{
	close(sock);
}

void firc::send_data(const char *data)
{
	if(sock!=0)
	{
		send(sock,data,std::string(data).length(),0);
	}
	else
	{
		throw new firc_error("Attempted to send data whilst not connected!\n");
	}
}

void firc::irc_connect()
{
	struct sockaddr_in host_addr;
	struct hostent *hent = gethostbyname(server.c_str());
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(port);
	host_addr.sin_addr = *((struct in_addr*)hent->h_addr);
	
	if(connect(sock,(struct sockaddr*)&host_addr,sizeof(struct sockaddr_in))<0)
	{
		throw new firc_error("Couldn't connect to server\n");
	}
}

bool firc::active()
{
	if(sock>0)
	{
		char buffer[1024] = { '\0' };
		int len = 0;
		if((len = recv(sock,buffer,1024,MSG_PEEK))>0)
		{
			std::string str_buf("");
			if(recv(sock,buffer,len,0)!=len)
			{
				return 0;
			}
			str_buf = buffer;
			if(str_buf.find("PRIVMSG")!=std::string::npos)
			{
				if(on_privmsg!=NULL)
				{
					std::string nick, channel, message;
					try
					{
						nick = str_buf.substr(1,str_buf.find("!")-1);
					}
					catch(...)
					{
						std::cout << "Caught an exception trying to find someone's nick" << std::endl;
						return 0;
					}
					try
					{
						if(str_buf.find("#")!=std::string::npos && str_buf.find("#")>str_buf.find(" :"))
							channel = str_buf.substr(str_buf.find("#"),str_buf.find(" :")-str_buf.find("#"));
						else
							channel = "";
					}
					catch(...)
					{	
						std::cout << "Caught an exception trying to find the channel name in a PM" << std::endl;
						return 0;
					}
					try
					{
						message = str_buf.substr(str_buf.find(" :")+2,(str_buf.length()-2)-(str_buf.find(" :")+2));			
					}
					catch(...)
					{
						std::cout << "Caught an exception trying to retrieve the message from a privmsg" << std::endl;
						return 0;
					}
					on_privmsg(this, nick.c_str(), (channel.length()>0)?(channel.c_str()):(NULL), message.c_str());
					if(message.find(command_delim)!=std::string::npos && message.length()!=std::string(command_delim).length())
					{
						on_command(this,nick.c_str(),(channel.length()>0)?(channel.c_str()):(NULL),message.substr(std::string(command_delim).length(),message.find(" ")-std::string(command_delim).length()).c_str(),(message.find(" ")!=std::string::npos)?(message.substr(message.find(" ")+1,(message.length())-(message.find(" ")+1)).c_str()):(NULL));
					}
				}
			}
			else
			{
				if(on_raw_data!=NULL)
					on_raw_data(this, str_buf.c_str(),len);	
			}
		}
		else
		{
			if(len==0)
				return 0;
		}
	}
	return 1;
}

void firc::send_login(char *user, char *realname, char *pass)
{
	if(sock>0)
	{
		std::string login = "PASS ";
		login.append(pass);
		login.append("\r\n");
		send_data(login.c_str());
		login.clear();
		login = "USER ";
		login.append(user);
		login.append(" localhost remotehost :");
		login.append(realname);
		login.append("\r\n");
		send_data(login.c_str());
	}
}

void firc::send_nick(char *new_nick)
{
	if(sock>0)
	{
		std::string request = "NICK ";
		request.append(new_nick);
		request.append("\r\n");
		send_data(request.c_str());
		nick = new_nick;
	}
}

void firc::send_join(const char *channel)
{
	if(sock>0)
	{
		std::string request = "JOIN ";
		request.append(channel).append("\r\n");
		send_data(request.c_str());
	}
}

void firc::send_privmsg(const char *recip, const char *message)
{
	if(recip==NULL)
		throw new firc_error("Send_privmsg was sent a NULL recipient!");
	std::string data("PRIVMSG ");
	data.append(recip).append(" :").append(message).append("\r\n");	
	send_data(data.c_str());
}

void firc::send_action(const char *channel, const char *action)
{
	if(channel==NULL)
		throw new firc_error("Send_action was sent a NULL channel!");
	std::string action_str("PRIVMSG ");
	action_str.append(channel);
	action_str.append(" :\x1");
	action_str.append("ACTION ");
	action_str.append(action);
	action_str.append("\x1");
	action_str.append("\r\n");
	send_data(action_str.c_str());
}
void firc::send_part(const char *channel)
{
	send_data(std::string("PART ").append(channel).append("\r\n").c_str());
}

void firc::send_part(const char *channel, const char *reason)
{
	send_data(std::string("PART ").append(channel).append(" :").append(reason).append("\r\n").c_str());
}

void firc::send_quit()
{
	send_data("QUIT\r\n");
}

void firc::send_quit(const char *reason)
{
	send_data(std::string("QUIT :").append(reason).append("\r\n").c_str());
}
