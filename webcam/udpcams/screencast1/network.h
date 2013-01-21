#ifndef __NETWORK_H__
#define __NETWORK_H__

extern void network_init(char *host);
extern int network_encode_data(void *data, unsigned char **out);

#endif

