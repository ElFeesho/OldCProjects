/*
   Copyright Christopher "Feesh" Sawczuk 2006 (c)
	
   This file is part of Nonlogic Pad Server.

   Nonlogicpad is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   Nonlogic Pad is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Nonlogic Pad; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <signal.h>
#include "misc.h"
#include "client.h"

int sock;

void signalHandler(int signum)
{
	writeToLog("Server Ended, caught a signal",LOG_ERROR);
	close(sock);
	exit(1);
}

int main(int argc, char **argv)
{
	printf("Nonlogic Pad Server Started\n");
	//Socket list used with select()
	signal(SIGINT,signalHandler);
	signal(SIGTERM,signalHandler);
	signal(SIGHUP,signalHandler);
	signal(SIGSTOP,signalHandler);
	fd_set cons;
	fd_set exceps;
	writeToLog("Server started",LOG_INFO);
	sock = socket(PF_INET,SOCK_STREAM,0);
	struct sockaddr_in local;
	local.sin_family = PF_INET;
	local.sin_port = htons(10101);
	local.sin_addr.s_addr = INADDR_ANY;
	memset(local.sin_zero,'\0',8);
	int res;
	int msg = 0;
	while((res = bind(sock,(struct sockaddr*)&local,sizeof(local)))!=0)
	{	
		if(!msg)
		{
			printf("Couldn't bind, retrying till bound\n");
			msg = 1;
		}
	}
	if(msg)
		printf("Done\n");
	setNonBlock(sock);
	int yes=1;
	if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) 
	{
		perror("setsockopt");
   	exit(1);
	} 
	listen(sock,255);
	// Start listening, FOR A MAXIMUM OF 255 CONNECTIONS!!!!?!
	int sockHigh = sock+1;
	cList *temp;
	unsigned char *ndata = challoc(255,'\0');
	for(;;) // Inifinite loop, better than while(1) because while(1) checks if 1 is still 1
	{
		FD_ZERO(&cons); //Reset lists , otherwise select doesn't work properly
		FD_ZERO(&exceps);
		FD_SET(sock,&exceps);
		FD_SET(sock,&cons);
		updateFDSet(&cons, &sockHigh);
		updateFDSet(&exceps, &sockHigh);
		int am = select(sockHigh,&cons,0,&exceps,0); //Set am, to store the amount of sockets that need reading from
		if(am>0)
		{
			if(FD_ISSET(sock,&cons))
			{
				struct sockaddr_in addr;
				unsigned int size = sizeof(struct sockaddr_in);
				int nid = accept(sock,(struct sockaddr*)&addr,&size); // Accept new incoming connections
				if(nid!=-1)
				{
					setNonBlock(nid);
					sockHigh = nid+1;
					addClient(nid);
					char *client = malloc(16);
					memset(client,'\0',16);
					getnameinfo((struct sockaddr*)&addr, size, client, 16,NULL,0,0);
					writeToLog("Client Joined",LOG_INFO);
					//sendXClientList(nid); Don't send client list since the channel the client is in isn't known
					free(client);
				}
			}
			if(clientList==NULL)
				continue; // Don't run through if no one is connected (it's just silly)
			for(temp=clientList;temp!=NULL;temp=temp->next) // run through the linked list
			{
				if(FD_ISSET(temp->id,&cons)) // Check if the current list item has data to read
				{
					char header;
					int bytes = recv(temp->id,&header,1,0);
					if(!bytes)
					{
						temp->conn = 0;
						writeToLog("Client exitted abnormally",LOG_WARN);
						close(temp->id);
						unsigned char ndata[4];
                  sprintf((char*)ndata,"\x04%03d",temp->id);
                  sendXClient(temp->id,ndata,4);
						removeClient(temp->id);
						continue;
					}
					if(header==0x01) //Line data
					{
						recv(temp->id,ndata,29,0);
						unsigned char sdata[33];
						memset(sdata,'\0',33);
						sprintf((char*)sdata,"\x01%03d",temp->id);
						memcpy(sdata+4,ndata,29);
						sendXClient(temp->id,sdata,33);
					}
					if(header==0x0A) //Rect (merge this and line together?
					{
						recv(temp->id,ndata,23,0);
						unsigned char sdata[27] = { 0 };
						sprintf((char*)sdata,"\x0A%03d",temp->id);
						memcpy(sdata+4,ndata,23);
						sendXClient(temp->id,sdata,27);
					}
					if(header==0x0B) //Bucket line
					{
						recv(temp->id,ndata,14,0);
						unsigned char sdata[18];
						memset(sdata,'\0',12);
						sprintf((char*)sdata,"\x0B%03d",temp->id);
						memcpy(sdata+4,ndata,14);
						sendXClient(temp->id,sdata,18);
					}
					if(header==0x04) // Disconnect
					{
						unsigned char ndata[4];
						sprintf((char*)ndata,"\x04%03d",temp->id);
						sendXClient(temp->id,ndata,4);
						temp->conn = 0;
						close(temp->id);
						writeToLog("Client disconnected",LOG_INFO);
						removeClient(temp->id);
						continue;
					}
					if(header==0x06) // Cleared the board
					{
						unsigned char ndata[4];
						sprintf((char*)ndata,"\x06%03d",temp->id);
						sendXClient(temp->id,ndata,4);
					}
					if(header==0x07) // Cleared the board
					{
						unsigned char ndata[4];
						sprintf((char*)ndata,"\x07%03d",temp->id);
						sendXClient(temp->id,ndata,4);
					}
					if(header==0x09) //Name change
					{
						recv(temp->id,temp->name,16,0);
						char changed = 0;
						if(checkXClientName(temp->id,temp->name))
						{
							do
							{
								if(strlen(temp->name)<16)
									strcat(temp->name,"_");
								else
									strncpy(temp->name,"NickCollision",16);
							}
							while(checkXClientName(temp->id,temp->name));
							changed = 1;
						}
						writeToLog("Client changed nick",LOG_INFO);
						unsigned char nname[20];
						sprintf((char*)nname,"\x09%03d%s",temp->id, temp->name);
						sendXClient(temp->id,nname,20);
						if(changed)
						{
							printf("Name changed\n");
							sprintf((char*)nname,"\x09%03d%s",-1,temp->name);
							sendClient(temp->id,nname,20);
						}
					}
					if(header==0x08) //Connection name change
					{
						recv(temp->id,temp->name,16,0);
						char changed = 0;
						if(checkXClientName(temp->id,temp->name))
						{
							do
							{
								if(strlen(temp->name)<16)
									strcat(temp->name,"_");
								else
									strncpy(temp->name,"NickCollision",16);
							}
							while(checkXClientName(temp->id,temp->name));
							changed = 1;
						}
						writeToLog("Client changed nick",LOG_INFO);
						unsigned char nname[20];
						sprintf((char*)nname,"\x03%03d%s",temp->id, temp->name);
						sendXClient(temp->id,nname,20);
						if(changed)
						{
							printf("Name changed\n");
							sprintf((char*)nname,"\x09%03d%s",-1,temp->name);
							sendClient(temp->id,nname,20);
						}
					}
					if(header==0x02) //Send message
					{
						char curChar = 0;
						char *msg = malloc(256);
						memset(msg,'\0',256);
						recv(temp->id,&curChar,1,0);
						if(curChar!=2)
							continue; //Duff message
						while(curChar!=3)
						{
							recv(temp->id,&curChar,1,0);
							if(curChar!=3)
								sprintf(msg,"%s%c",msg,curChar);
						}
						unsigned char* nmsg = malloc(258);
						memset(nmsg,'\0',258);
						int len = sprintf((char*)nmsg,"\x02%03d\x02%s\x03",temp->id,msg);
						sendXClient(temp->id,nmsg,len);
						free(nmsg);
						free(msg);
					}
					if(header==0x0C) //Send action
					{
						char curChar = 0;
						char *msg = malloc(256);
						memset(msg,'\0',256);
						recv(temp->id,&curChar,1,0);
						if(curChar!=2)
							continue; //Duff action
						while(curChar!=3)
						{
							recv(temp->id,&curChar,1,0);
							if(curChar!=3)
								sprintf(msg,"%s%c",msg,curChar);
						}
						unsigned char* nmsg = malloc(258);
						memset(nmsg,'\0',258);
						int len = sprintf((char*)nmsg,"\x0C%03d\x02%s\x03",temp->id,msg);
						sendXClient(temp->id,nmsg,len);
						free(nmsg);
						free(msg);
					}
					if(header==0x0D) // Changed channel
					{
						printf("Client changed channel: %d\n",temp->id);
						char *buffer = malloc(16);
						recv(temp->id,buffer,16,0);
						if(strcmp(temp->channel,buffer)==0)
						{
							//Same channel don't do anything
							continue;
						}
						else
						{
							unsigned char msgBuff[4];
							sprintf((char*)msgBuff,"\x04%03d",temp->id);
							sendXClient(temp->id,msgBuff,4);
							strcpy(temp->channel,buffer);
							unsigned char nname[20];
	                  sprintf((char*)nname,"\x03%03d%s",temp->id, temp->name);
   	               sendXClient(temp->id,nname,20);
							sendXClientList(temp->id);
						}		
					}

				}
				if(FD_ISSET(temp->id,&exceps))
				{
					printf("Client %d has disconnected or performed an error\n",temp->id);
					//This doesn't work :( and it makes me cry
				}
			}
		}	
		memset(ndata,'\0',255);	
	}
	return 0;
}
