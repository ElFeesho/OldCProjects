#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char **argv)
{
	printf("Feesh UDP Server running on port 7334\n");
	
	int bind_sock = socket(AF_INET,SOCK_DGRAM,0);
	struct sockaddr_in serv_info;
	serv_info.sin_family = AF_INET;
	serv_info.sin_port = htons(7443);
	serv_info.sin_addr.s_addr = INADDR_ANY;
	
	if(bind(bind_sock,(struct sockaddr*)&serv_info,sizeof(struct sockaddr))==-1)
	{
		perror("Couldn't bind the socket");
		exit(1);
	}
	while(1)
	{
		char *buffer = malloc(32);
		struct sockaddr_in who;
		socklen_t len = sizeof(struct sockaddr);
	
		if(recvfrom(bind_sock,buffer,32,0,(struct sockaddr*)&who,&len)!=-1)
			printf("Received: %s\n",buffer);
	}
	close(bind_sock);
	return 0;
}
