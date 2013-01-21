#ifndef __NET_H__
#define __NET_H__

int net_connect(const char *username, const char *password);
const char *net_update();

void net_shutdown();

#endif
