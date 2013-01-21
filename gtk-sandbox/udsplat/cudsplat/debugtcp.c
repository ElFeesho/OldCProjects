#include <stdio.h>
#include <string.h>

#include <sys/socket.h>
#include <netdb.h>

int udp_sock;
struct sockaddr_in udp_sock_addr;

int main(int argc, char **argv)
{
   udp_sock = socket(AF_INET, SOCK_STREAM, 0);

   udp_sock_addr.sin_family = AF_INET;
   udp_sock_addr.sin_port = htons(18093);
   memset(udp_sock_addr.sin_zero, 0, 8);
   udp_sock_addr.sin_addr.s_addr = INADDR_ANY;
   
   bind(udp_sock, (struct sockaddr*)&udp_sock_addr, sizeof(udp_sock_addr));

	listen(udp_sock, 10);
	
	int incoming = accept(udp_sock,NULL,0);
	printf("Got a connection\n");
	for(;;)
	{
		char buffer[1024] = { 0 };
		recv(incoming, buffer, 1024, 0);
		printf("Data: %s\n",buffer);
		fflush(stdout);
	}

	return 0;
}
