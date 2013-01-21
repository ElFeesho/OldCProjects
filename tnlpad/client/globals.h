#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#include <gnet.h>

extern char globals_nick[16];
extern char globals_name[32];
extern char globals_website[32];
extern char globals_email[32];
extern char globals_server[128];
extern char globals_channel[16];
extern char globals_connected;
extern char globals_stroke_width;
extern char globals_has_ink;
extern char globals_has_voice;
extern char globals_has_ops;
extern unsigned short globals_port;
extern unsigned short globals_red;
extern unsigned short globals_green;
extern unsigned short globals_blue;
extern GTcpSocket *globals_tcp_sock;
extern GIOChannel *globals_sock_chan;

extern char globals_input_ext;

extern void globals_init();

#endif
