#ifndef __NETWORK_H__
#define __NETWORK_H__

void network_init(void);
void network_change_ip(char *ip);
void network_send(char *who, char *message);
#endif
