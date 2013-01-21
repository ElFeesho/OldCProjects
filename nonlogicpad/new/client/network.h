#ifndef __NETWORK_H__
#define __NETWORK_H__

extern void network_connect();
extern void network_disconnect(int code);
extern void network_send_line(int x, int y, int x2, int y2);
extern void network_send_erase(int x, int y, int x2, int y2);
extern void network_send_rect(int x, int y, int x2, int y2);
extern void network_send_fill(int x, int y);
extern void network_send_clear();
extern void network_send_message(char *message);
extern void network_send_action(char *action);
extern void network_send_nick();
extern void network_send_channel();
extern void network_send_topic(char *topic);

#endif
