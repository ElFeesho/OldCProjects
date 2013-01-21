#ifndef __SOCKET_HPP__
#define __SOCKET_HPP__

#include <string>

using namespace std;

class Socket
{
public:
	Socket();
	Socket(int sock);
	~Socket();

	void connect(string host, int pNum);
	bool listen(unsigned short pNum);
	Socket *accept();
	int getSocket();

	void setBlocking(bool block);

	void shutdown();

	int send(const char *data, int len);
	int send(char data);
	char recv();
	char *recv(int len);
private:
	int __sock;
};

#endif
