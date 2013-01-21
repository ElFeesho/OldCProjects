#include <stdlib.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include "log.h"

static int network_listen_sock;

void network_init(short port)
{
	network_listen_sock = socket(AF_INET, SOCK_STREAM,0);
	
	struct sockaddr_in network_host;
	struct hostent *network_hent = gethostbyname("localhost"); //is this bad?
	
	network_host.sin_family = AF_INET;
	network_host.sin_port = htons(port);
	
}
