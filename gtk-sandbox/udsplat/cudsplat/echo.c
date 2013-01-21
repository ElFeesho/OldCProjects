#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

#include <netdb.h>

int udp_sock;
struct sockaddr_in dst;

int main(int argc, char **argv)
{
	udp_sock = socket(PF_INET,SOCK_DGRAM,0);
	dst.sin_family = PF_INET;
	dst.sin_port = htons(18093);
	dst.sin_addr.s_addr = INADDR_ANY;

	sendto(udp_sock, "Hello\n",6,0,(struct sockaddr*)&dst,sizeof(struct sockaddr));
	close(udp_sock);
	return 0;
}
