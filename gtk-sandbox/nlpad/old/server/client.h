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


#ifndef __CLIENT_H__
#define __CLIENT_H__
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
	int id;
	char conn;
	char name[16];
	char channel[16];
	void *next;
}cList;

cList *clientList; //Hidden backbone

void updateFDSet(fd_set *set, int *highSock);
void addClient(int id);
void sendXClient(int id,unsigned char *data, int size);
void sendClient(int id,unsigned char *data, int size);
void sendXClientList(int id);
void removeClient(int id);
char checkXClientName(int id, char *name);
cList *getClient(int id);
#endif
