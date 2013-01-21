#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <glib.h>
#include "nldttp.h"

int main(int argc, char ** argv)
{
	printf("UDP NLDTTP Server Test\n");
	
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_port = htons(20202);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	
	int sock = socket(AF_INET,SOCK_DGRAM,0);
	
	bind(sock,(struct sockaddr*)&server,sizeof(struct sockaddr_in));
	FILE *file_rec = fopen("test.bin","w");
	for(;;)
	{
		nldttp_header_t *header = NULL;
		struct sockaddr client;
		int sockaddr_len = sizeof(struct sockaddr);
		if((header = nldttp_header_read(sock))!=NULL)
		{
			printf("Received Packet: %d of %d\r",header->packet_num, header->packet_tot);
			void *data = g_malloc0(header->size+10);
			if(recvfrom(sock,data,header->size+10,0,&client,&sockaddr_len)==header->size+10)
			{
				fwrite(data+10,header->size,1,file_rec);
				if(header->packet_num == header->packet_tot)
				{
					fclose(file_rec);
					printf("\nReceived a file!\n");
				}
			}
			else
			{
				printf("\nDidn't receive all the data!\n");
			}
		}
		sendto(sock,"GOT",3,0,&client,sockaddr_len);
	}
	close(sock);
	return 0;
}
