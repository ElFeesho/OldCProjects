#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <gnet.h>

extern char globals_nick[16];
extern char globals_server[128];
extern char globals_channel[32];
extern char globals_connected;
extern char globals_stroke_width;
extern unsigned short globals_port;
extern unsigned short globals_red;
extern unsigned short globals_green;
extern unsigned short globals_blue;
extern GTcpSocket *globals_tcp_sock;
extern GIOChannel *globals_sock_chan;
extern void globals_init();

#endif
