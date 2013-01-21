#include <stdio.h>
#include <string.h>
#include <mimic.h>
#include <sys/socket.h>
#include <netdb.h>

static MimCtx *ctx;
static int fd = -1;
static struct sockaddr_in dst;
void network_send(int size, char *out);

void network_init(char *host)
{
	ctx = mimic_open();
	mimic_encoder_init(ctx, MIMIC_RES_HIGH);
	fd = socket(AF_INET,SOCK_DGRAM,0);
	struct hostent *hent = gethostbyname(host);
	if(!hent)
	{
		fprintf(stderr, "Failed resolving host name! - Check it exists\n");
		return;
	}
	
	dst.sin_family = AF_INET;
	dst.sin_port = htons(3456);
	dst.sin_addr = *((struct in_addr*)hent->h_addr);
	memset(dst.sin_zero, '\0', 8);
}

int network_encode_data(void *data, unsigned char **out)
{
	int rval = 0;
	*out = g_malloc0(320*240*3);
	mimic_encode_frame(ctx, (const guchar*)data, (guchar*)*out,&rval, TRUE);
	network_send(rval,*out);
	return rval;
}

void network_send(int size, char *out)
{
	sendto(fd,&size,4,0,(struct sockaddr*)&dst,sizeof(struct sockaddr_in));
	sendto(fd,out,size,0,(struct sockaddr*)&dst,sizeof(struct sockaddr_in));
}
