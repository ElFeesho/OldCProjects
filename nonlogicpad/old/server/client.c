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


#include "client.h"
#include <sys/socket.h>
#include <errno.h>

void removeClient(int id)
{
	cList *temp = 0;
	if(clientList->id == id)
	{
		//First client to delete
		temp = clientList;
		if(temp->next!=NULL)
		{
			clientList = temp->next;
			free(temp);
		}
		else 
		{
			free(clientList);
			clientList = NULL;
			printf("No clients on server! Entering hybernate mode!\n");
		}
		return;
	}
	cList *prev = 0;
	temp = clientList;
	while(temp)
	{
		if(temp->id == id)
		{
			if(prev!=NULL && temp->next !=NULL)
			{
				prev->next = temp->next;
			}
			else
			{
				prev->next = NULL;
			}
			free(temp);
			return;
		}
		prev = temp;
		temp = temp->next;
	}
	printf("Client not found to remove!\n");
	return;
}

void updateFDSet(fd_set *set, int *highSock)
{
	int highestSock = 0;
	if(clientList!=NULL)
	{
		cList *temp = clientList;
		for(;temp!=NULL;temp = temp->next)
		{
			if(temp->conn)
			{
				int id = temp->id;
				if(id > highestSock-1) highestSock = id+1;
				FD_SET(id,set);
			}
		}
		*highSock = highestSock;
	}
}

void addClient(int id)
{
	if(clientList == NULL)
	{
		clientList = (cList*)malloc(sizeof(cList));
		clientList->id = id;
		clientList->conn = 1;
		clientList->next = 0;
	}else{
		cList *new = (cList*)malloc(sizeof(cList));
		new->id = id;
		new->next = 0;
		new->conn = 1;
		cList *temp = clientList;
		while(temp->next!=NULL)
			temp = temp->next;
		temp->next = new;
	}
}


void sendXClient(int id,unsigned char *data, int size)
{
	cList *temp = clientList;
	for(;temp!=NULL;temp=temp->next)
	{
		if(temp->id != id && temp->conn && strcmp(temp->channel,getClient(id)->channel)==0)
		{
			send(temp->id,data,size,0);
		}
	}
}

void sendClient(int id,unsigned char *data, int size)
{
	cList *temp = clientList;
	for(;temp!=NULL;temp=temp->next)
	{
		if(temp->id == id && temp->conn)
		{
			send(temp->id,data,size,0);
		}
	}
}

void sendXClientList(int id)
{
	cList *temp = clientList;
	unsigned char *msg = malloc(32);
	if(clientList!=NULL)
	{
		for(;temp!=NULL;temp=temp->next)
		{
			if(temp->id != id && temp->conn && strcmp(temp->channel,getClient(id)->channel) == 0)
			{
				sprintf((char*)msg,"\x09%03d%s",temp->id,temp->name);			
				send(id,msg,20,0);
			}
		}
	}
	free(msg);
}

char checkXClientName(int id, char *name)
{
	cList *temp = clientList;
	if(clientList!=NULL)
	{
		for(;temp!=NULL;temp=temp->next)
		{
			if(temp->id != id && temp->conn)
			{
				printf("IF %s == %s \n",temp->name, name);
				if(strcmp(temp->name,name)==0)
				{
					return 1;
				}
			}
		}
	}
	return 0;
}

cList *getClient(int id)
{
	cList *temp = clientList;
	while(temp!=NULL)
	{
		if(temp->id == id)
			return temp;
		else
			temp = temp->next;
	}
	return NULL;
}
