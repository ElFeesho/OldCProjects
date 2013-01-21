#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <netdb.h>
#include <sys/socket.h>

int con_new(char *host)
{
	/* First of all, resolve the host */
	struct hostent *host_resolv = gethostbyname(host);

	/* 
		If the host isn't valid host_resolv will be null
	*/
	if(!host_resolv)
	{
		fprintf(stderr, "Failed resolving %s - check network status\n",host);
		return -1;
	}

	struct sockaddr_in g_addr;
	g_addr.sin_family = AF_INET;
	/* 
		htons = host to network short, converting endians if needed 
		Turns out network endian is big endian, whilst here we are
		(us intellers and amders) with little endians... least our
		processors are fast to make up for it!!!
	*/
	g_addr.sin_port = htons(80);
	g_addr.sin_addr = *((struct in_addr*)host_resolv->h_addr);

	/*
		Padding the sockaddr_in with some 0s now, forget why,
		doesn't really make a difference if you don't pad it.
	*/
	memset(g_addr.sin_zero, '\0', 8);
	
	/* Create the socket, of family AF_INET, of type, SOCK_STREAM (tcp), with 0 flags */
	int sock = socket(AF_INET,SOCK_STREAM,0);	

	/* Connect will block till a connection is established typically- only timesout after a long time */
	if(connect(sock, (struct sockaddr*)&g_addr,sizeof(struct sockaddr))<0)
	{
		fprintf(stderr, "Failed to connect to %s.\n",host);
		return -1;
	}

	return sock;
}

void con_send_data(int con, void *data, int len)
{
	/* Lame wrapper function */
	if(send(con,data,len,0)!=len)
	{
		fprintf(stderr,"Failed sending data - byte count mismatch\n");
	}
}

char *con_recv_data(int con)
{
	char *data = malloc(32768);
	char *chunk = malloc(1024);
	int chunks = 0;
	memset(chunk,'\0', 1024);
	while(chunks<11)
	{
		recv(con,chunk,1024,0);
		sprintf(data,"%s%s",data,chunk);
		chunks++;
		memset(chunk,'\0',1024);
	}
	return data;
}
