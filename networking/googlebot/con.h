#ifndef __CON_H__
#define __CON_H__

int con_new(char *host);
void con_send_data(int con, void *data, int len);
char *con_recv_data(int con);
#endif
