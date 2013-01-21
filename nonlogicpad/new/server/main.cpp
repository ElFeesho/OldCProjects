/* Where all the magic really happens */

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netdb.h>

#include <signal.h>

#include "log.hpp"
#include "network.hpp"
#include "client.hpp"

using namespace std;

void signal_catcher(int signal) /* not quite a dream catcher */
{
	/* always close the log - flushes anything to the file that hasn't been flushed */
	cout << "Caught signal, ending" << endl;
	log_write("SIGNAL CAUGHT\n");
	log_close();
	exit(1);
}

int main(int argc, char ** argv)
{
	cout << "NLPad Server V1.0.1" << endl;
	/* Begin logging */
	log_open();
	
	signal(SIGINT,signal_catcher);
	signal(SIGQUIT,signal_catcher);
	signal(SIGSEGV,signal_catcher);
	signal(SIGTERM,signal_catcher);
	
	NP_Network server(10101);
	
	
	
	
	/* Begin Receiving information */
	for(;;)
	{
		printf("Looping\n");
		int high_sock = server.update_socks();
		printf("Updated socks\n");
		int amount = select(high_sock, server.get_sock_set(),0, 0, 0);
		printf("Selecting\n");
		if(amount > 0)
		{
			if(FD_ISSET(server.get_sock(),server.get_sock_set()))
			{
				server.accept_clients();
				server.update_socks();
			}
			
			for(std::vector<NP_Client*>::iterator it = server.get_clients()->begin(); it!=server.get_clients()->end(); it++)
			{
				if(FD_ISSET((*it)->get_sock(),server.get_sock_set()))
				{
					char header;
					if(recv((*it)->get_sock(),&header,1,0)!=1)
					{
						log_write("Client misread! ID:\n");
						continue;
					}
					cout << "Header: " << header << endl;
					int pack_length = recv((*it)->get_sock(),(void*)(new char[1024]),1024,MSG_PEEK);
					cout << "Data length: "<< pack_length << endl;
					switch(header)
					{
						case 0x01: //Draw Line
							if((*it)->get_ink())
							{
								cout << "Got Line" << endl;
								unsigned char *rec_data = new unsigned char[pack_length];
								recv((*it)->get_sock(),(void*)rec_data,pack_length,0);
								unsigned char *data = new unsigned char[pack_length+4];
								//sprintf((char*)data,"\x1%03d",(*it)->get_sock());
								//memcpy((void*)(data+4),(void*)(rec_data,29));
								//client_send_all_but(temp,data,33);
								delete data;
								delete rec_data;
							}
							break;
						case 0x02: //Write message
							break;
						case 0x03: //Client connect
							break;
						case 0x04: //Client disconnect
							break;
						case 0x05: //Client changed channel
							break;
						case 0x06: //Board cleanup request
							break;
						case 0x07: // Unused now
							break;
						case 0x08:
						{
							log_write("Got login nick\n");
							cout << "Got login nick - " << pack_length << endl;
							unsigned char *data = new unsigned char[pack_length];
							recv((*it)->get_sock(),(void*)data,pack_length,0);
							(*it)->set_nick((char*)data);
							delete data;
						}
							break;
						case 0x09: //Nick Change
							break;
						case 0x0A: //Rectangle data
							break;
						case 0x0B: //Bucket Fill
							break;
						case 0x0C: //Action received
							break;
						case 0x0D: //Channel Change
						{
							cout << "Changing channel" << endl;
							unsigned char *data = new unsigned char[pack_length];
							recv((*it)->get_sock(),(void*)data,pack_length,0);
							//client_change_channel((*it)->get_sock(),(char*)data);
							//unsigned char *send_data = new unsigned char[20];
							//memset(send_data,0x0,20);
							//sprintf((char*)send_data,"\x3%03d%s",temp->id,temp->nickname);
							//client_send_all_but(temp,send_data,20);
						}
							break;
						case 0x0E: //Topic Change
							break;
						default:
							log_write("Garbled header / unrecognized command - disconnecting\n");
							it = server.get_clients()->erase(it);
							break;
					}
				}
			}
		}
		else
		{
			cout << "Nothing to do" << endl;
		}
	}
	return 0;
}
