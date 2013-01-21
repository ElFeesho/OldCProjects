#ifndef __NETWORK_H__
#define __NETWORK_H__

void net_login(char *email, char *password);

gboolean net_connected(void);
void net_send_nick(char *nick);
void net_close();

#endif
