#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>

static void usage();

static void check_file(const char *filename);

static unsigned int get_file_size(const char *filename);

static int create_socket(const char *server);

int main(int argc, char **argv)
{
	if(argc!=3)
	{
		usage();
		exit(1);
	}
	check_file(argv[1]);
	
	unsigned int file_size = get_file_size(argv[1]);
	
	int server_socket = create_socket(argv[2]);
	while(1)
	{
		char *data_buffer = malloc(32);
		fgets(data_buffer,32,stdin);
		printf("Sending\n");
		send(server_socket,data_buffer,strlen(data_buffer),0);
	}
	close(server_socket);
	return 0;
	
}

void usage()
{
 	printf("Usage: fudpcl file server\n");
}

void check_file(const char *filename)
{
	//Wanna make sure the file exists
	FILE *file_to_send = fopen(filename,"r");
	if(!file_to_send)
	{
		perror("Couldn't open file");
		exit(1);
	}
	fclose(file_to_send);
}

unsigned int get_file_size(const char *filename)
{
	FILE *file_to_send = fopen(filename,"r");
	if(!file_to_send)
	{
		perror("Couldn't open file to check for size");
		exit(1);
	}
	unsigned int file_size = 0;
	fseek(file_to_send,0,SEEK_END);
	file_size = ftell(file_to_send);
	fclose(file_to_send);
	return file_size;
}

int create_socket(const char *server)
{
	int ret_sock = socket(AF_INET,SOCK_DGRAM,0);
	struct hostent *hent = gethostbyname(server);
	struct sockaddr_in endpoint; //So what it's not USB dev but damn it! it's an endpoint
	endpoint.sin_family = AF_INET;
	endpoint.sin_port = htons(7335);
	endpoint.sin_addr = *((struct in_addr*)hent->h_addr);
	memset(&(endpoint.sin_zero),'\0',8);
	if(bind(ret_sock,(struct sockaddr*)&endpoint,sizeof(struct sockaddr))==-1)
	{
		perror("Couldn't bind the socket to the server");
		exit(1);
	}
	return ret_sock;
}
