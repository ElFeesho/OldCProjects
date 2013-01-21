#ifndef __NETWORK_H__
#define __NETWORK_H__

extern void network_connect();
extern void network_disconnect(int code);
extern void network_send_line(int x, int y, int x2, int y2);
extern void network_send_erase(int x, int y, int x2, int y2);
extern void network_send_rect(int x, int y, int x2, int y2);
extern void network_send_circle(int x, int y, int x2, int y2);
extern void network_send_fill(int x, int y);
extern void network_send_clear();
extern void network_send_message(char *message);
extern void network_send_action(char *action);
extern void network_send_nick();
extern void network_send_channel();
extern void network_send_register(char *pass);
extern void network_send_login(char *pass);
extern void network_send_topic(char *topic);
extern void network_send_lock_chan();
extern void network_send_lock_topic();
extern void network_send_take_voice(char id);
extern void network_send_take_ink(char id);
extern void network_send_give_voice(char id);
extern void network_send_give_ink(char id);

#endif
