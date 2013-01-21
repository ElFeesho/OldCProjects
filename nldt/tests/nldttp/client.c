#include <stdio.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <math.h>
#include <netdb.h>
#include <glib.h>
#include "nldttp.h"

#define PACKET_SIZE 1024

int main(int argc, char **argv)
{
	printf("UDP Test Client for NLDTTP\n");
	if(argc<3)
	{
		fprintf(stderr,"Usage: %s server file\n", argv[0]);
		return 1;
	}
	struct sockaddr_in server;
	struct hostent *hent = gethostbyname(argv[1]);
	server.sin_family = AF_INET;
	server.sin_port = htons(20202);
	server.sin_addr = *((struct in_addr*)hent->h_addr);
	
	int sock = socket(AF_INET,SOCK_DGRAM,0);
	struct stat file_mode;
	if(lstat(argv[2],&file_mode)<0)
	{
		fprintf(stderr,"File not found: %s\n",argv[2]);
		return 1;
	}
	
	FILE *file_to_send = fopen(argv[2],"rb");
	short pack_num = 1;
	unsigned long int file_size = file_mode.st_size;
	while(pack_num<=ceil(file_mode.st_size/PACKET_SIZE))
	{
		void *file_buf = g_malloc0(PACKET_SIZE);
		nldttp_header_t *header = nldttp_header_new(0,0,pack_num,ceil(file_mode.st_size/PACKET_SIZE),0,0,(file_size-PACKET_SIZE>0)?(PACKET_SIZE):(file_size));
		printf("Filebuf: fread(%X, %d, 1, %X);\n",file_buf,(file_size-PACKET_SIZE>0)?(PACKET_SIZE):(file_size),file_to_send);
		fread(file_buf,(file_size-PACKET_SIZE>0)?(PACKET_SIZE):(file_size),1,file_to_send);
		file_size -= PACKET_SIZE;
		
		int send_size;
		void *send_data = nldttp_header_prefix(header, file_buf, &send_size);
		
		printf("Sending packet: %d\r",header->packet_num);
		if(sendto(sock, send_data, send_size, 0, (struct sockaddr*)&server, sizeof(struct sockaddr))==send_size)
		{	
		}
		else
		{
			fprintf(stderr,"Failed sending data\n");
			return 1;
		}
		char buf[4] = { '\0' };
		if(recvfrom(sock,&buf,3,0,NULL,0)==3)
		{
			pack_num++;
		}
		else
		{
			//Send a SYN to server for whether it got the last packet
			continue;
		}
		free(send_data);
		free(header);
		free(file_buf);
	}
	fclose(file_to_send);
	close(sock);
	return 0;
}
