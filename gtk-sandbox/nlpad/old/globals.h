#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <gnet.h>
#include <gdk/gdk.h>
#include <stdlib.h>

typedef struct _Globals
{
	/*char nick[16];
	char server[64];
	char channel[32];*/
	char *nick;
	char *server;
	char *channel;
	int toolMode;
	char snap;
	int32_t port;
	int32_t x, y;
	int32_t r,g,b;
	int32_t ticks;
	int32_t gridSize;
	char connected;
	GIOChannel *sockChan;
	GTcpSocket* tcpSock;
	void *tline;
	void *trect;
	char start;
	unsigned char alpha;
	char visible;
	GdkImage *image;
}Globals;

Globals globals;

void initGlobals();

#endif
