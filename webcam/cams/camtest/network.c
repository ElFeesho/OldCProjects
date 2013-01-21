#include <stdio.h>
#include <mimic.h>
#include <sys/socket.h>
#include <netdb.h>

static MimCtx *ctx;
static int fd = -1;
static int connected = 0;

void network_send(int size, char *out);

void network_init(char *host)
{
	ctx = mimic_open();
	mimic_encoder_init(ctx, MIMIC_RES_HIGH);

	fd = socket(AF_INET,SOCK_STREAM,0);
	struct hostent *hent = gethostbyname(host);
	struct sockaddr_in dst;
	dst.sin_family = AF_INET;
	dst.sin_port = htons(3456);
	dst.sin_addr = *((struct in_addr*)hent->h_addr);
	
	if(connect(fd,(struct sockaddr*)&dst,sizeof(dst))<0)
		printf("Failed connecting...\n");
	else
		connected = 1;
}

int network_encode_data(void *data, unsigned char **out)
{
	if(!connected)
		return 0;
	int rval = 0;
	*out = g_malloc0(320*240*3);
	mimic_encode_frame(ctx, (const guchar*)data, (guchar*)*out,&rval, TRUE);
	network_send(rval,*out);
	return rval;
}

void network_send(int size, char *out)
{
	send(fd,&size,4,0);
	send(fd,out,size,0);
}
