#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <sys/socket.h>
#include <netdb.h>

char initialiseNetwork();
int openConnection();
void sendData(char *data);
char *recvData();
void handleData(char *data);
void sendAuth();
void shutdownCon();

//GLOBALS

int mainSock;
struct sockaddr_in sAddr;
struct hostent *hent;

//END GLOBALS

#endif
