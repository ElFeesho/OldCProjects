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


#include <stdlib.h>
#include <string.h>
#include "misc.h"

unsigned char *mkMsg(char head, int id, char *message,unsigned char *len)
{
	char *retMsg = malloc(strlen(message)+5);
	*len = strlen(message);
	sprintf(retMsg,"%c%03d%c%s",head,id,*len,message);
	*len+=5;
	return (unsigned char*)retMsg;
}

void *challoc(int bytes, char val)
{
	void *ret = (void*)malloc(bytes);
	memset(ret,val,bytes);
	return ret;
}

void writeToLog(char *message, char priority)
{
	FILE *logFile = fopen("log.txt","a"); //Create if necessary
	fseek(logFile,0,SEEK_END); //Goto end of file
	char *output = challoc(256,'\0');
	switch(priority)
	{
		case LOG_ERROR: //Error
			strcpy(output,"(!!): ");
			break;
		case LOG_WARN: //Warning
			strcpy(output,"(WW): ");
			break;
		case LOG_INFO: //Info
			strcpy(output,"(II): ");
			break;
		default:
			strcpy(output,"(\?\?): ");
			break;
	}
	strcat(output,message);
	strcat(output,"\n");
	fwrite(output,1,strlen(output),logFile);
	fclose(logFile);
	return;
}

void setNonBlock(int sock)
{
	int opts = fcntl(sock,F_GETFL,0);
	opts |= O_NONBLOCK;
	if(fcntl(sock,F_SETFL,opts)==0)
	{
		return; //Went down fine
	}else{
		perror("Nonblocking"); //Went down not so fine
		writeToLog("Nonblocking failed",0);
		exit(1);
		return;
	}
}
