#include <stdio.h>

#include <sys/socket.h>
#include <netdb.h>

#include <string.h>

static int sock = 0;

int net_connect(const char *username, const char *password)
{
	sock = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in sock_dst;

	sock_dst.sin_family = PF_INET;
	sock_dst.sin_port = htons(23);
	sock_dst.sin_addr.s_addr = inet_addr("192.168.1.254");

	if(connect(sock, (struct sockaddr*)&sock_dst, sizeof(sock_dst))!=0)
		return -1;

	// Otherwise, login time!
	send(sock, username, strlen(username),0);
	send(sock, "\r\n", 2, 0);

	send(sock, password, strlen(password),0);
	send(sock, "\r\n", 2, 0);
	sleep(1);
	// Clear the recv buffer
	char buffer[4096] = { 0 };
	recv(sock, buffer, 4096, MSG_DONTWAIT);

	return sock;
}

const char *net_update()
{
	send(sock, ":xdsl info\r\n", 12, 0);
	char buffer[4096] = { 0 };
	int i = 0;
	do
	{
		recv(sock,buffer+i,1,0);
	}
	while(buffer[i++]!='>');

	int k;
	for(k = 0;k<i;k++)
	{
		if(buffer[k]=='{')
		{
			buffer[k-2] = '\0';
			break;
		}
	}
	return strdup(buffer+12);
}

void net_shutdown()
{
	shutdown(sock, 0);
}
