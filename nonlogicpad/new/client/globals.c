#include <stdio.h>
#include <gnet.h>

char globals_nick[16] = { 0 };
char globals_server[64] = { 0 };
char globals_channel[32] = { 0 };
char globals_connected = 0;
char globals_stroke_width = 1;
unsigned short globals_port = 10101;
unsigned short globals_red = 0;
unsigned short globals_green = 0;
unsigned short globals_blue = 0;
GTcpSocket *globals_tcp_sock = NULL;
GIOChannel *globals_sock_chan = NULL;

void globals_init()
{
	//Empty for now
	globals_connected = 0;
}
