#include "network.h"
#include <stdlib.h>
#include <memory.h>
#include <stdio.h>
#include "plugins.h"
#include "strops.h"

char *botChannel = "#yipyeo";

char initialiseNetwork()
{
	mainSock = socket(AF_INET,SOCK_STREAM,0);
	if(mainSock<0)
		return 0;

	hent = gethostbyname("irc.freenode.net");
	if(!hent)
		return 0;

	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(6667);	
	sAddr.sin_addr = *((struct in_addr*)hent->h_addr);
	return 1;
}

int openConnection()
{
	if(connect(mainSock,(struct sockaddr*)&sAddr,sizeof(sAddr))<0)
		return 1;
	else
		return 0;
}

void sendData(char *data)
{
	if(mainSock>0)
	{
		send(mainSock,data,strlen(data),0);
	}
}

char *recvData()
{
	char *data = malloc(512);
	int amount = recv(mainSock,data,512,MSG_PEEK);
	free(data);
	data = malloc(amount);
	memset(data,'\0',amount);
	recv(mainSock,data,amount,0);
	return data;
}

void handleData(char *data)
{
	//:feesh!n=Feesh@unaffiliated/feesh PRIVMSG ##testbot :hello
	if(strpos(data,':',1)!=-1)
	{
		char fpos = strpos(data,':',1)+1;
		char epos = strpos(data,'!',1);
		char priv = 0;
		char *nick = substr(data,fpos,epos);
		char *msg = substr(data,strpos(data,':',2)+1,strpos(data,'\r',1));
		if(epos!=-1)
		{
			if(strcmp("yybot",substr(data,strpos(data,' ',2)+1,strpos(data,' ',3)))!=0)
			{
				printf("%s: %s\n",nick,msg);
			}
			else
			{
				printf("In Private %s: %s\n",nick,msg);
				priv = 1;
			}
			if(priv)
			{
				if(strcmp(nick,"feesh")==0)
				{
					if(strncmp(msg,"!rmmod",6)==0 && strlen(msg)>8)
					{
						if(removePlugs(msg+7))
						{
							sendData("PRIVMSG feesh :Removed the plugin successfully\r\n");
						}
						else
						{
							sendData("PRIVMSG feesh :Failed to remove the plugin\r\n");
						}
						return;
					}
					if(strncmp(msg,"!lsmod",6)==0)
					{
						sendData("PRIVMSG feesh :List of Plugins\r\n"); 
						plugList *temp = plugins;
						if(temp==NULL)
							sendData("PRIVMSG feesh :  No plugins loaded\r\n");
						while(temp!=NULL)
						{
							char *message = malloc(192);
							sprintf(message,"PRIVMSG feesh :  %s\r\n",temp->name);
							sendData(message);
							free(message);
							temp=temp->next;
						}
						sendData("PRIVMSG feesh :End of Plugin List\r\n");
						return;
					}
					if(strncmp(msg,"!insmod",7)==0)
					{
						if(!addPlugin(msg+8))
						{
							sendData("PRIVMSG feesh :Failed to load the plugin\r\n");
						}
						else
						{
							sendData("PRIVMSG feesh :Loaded the plugin successfully\r\n");
						}
					}
				}
			}
			if(plugins!=NULL)
			{
				plugList *temp = plugins;
				for(;temp!=NULL;temp = temp->next)
				{
					if((temp->plug->pluginFunc(botChannel,nick,msg,priv))!=NULL)
					{
						sendData(temp->plug->pluginFunc(botChannel,nick,msg,priv));
					}
				}
			}
		}
	}
	else
	{
		printf("%s",data);
	}
}

void sendAuth()
{
	sendData("USER yybot localhost remotehost :YYbot\r\n");
	sendData("NICK yybot\r\n");
	sendData("PASS zuzu\r\n");
	sendData("PRIVMSG nickserv :identify t3hp4ssw0rd\r\n");
	sendData("JOIN #yipyeo\r\n");
}

void shutdownCon()
{
	clearPlugs();
	printf("Closed plugins\n");
	shutdown(mainSock,SHUT_RDWR);
	exit(1);	
}
