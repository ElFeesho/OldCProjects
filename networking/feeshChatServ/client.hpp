#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include "socket.hpp"
#include <string>

using namespace std;

class Client
{
public:
	Client();
	Client(Socket *socket);
	~Client();
	int getID();
	void setID(int nid);

	string getName();
	void setName(string nName);

	Socket *getSocket();
	void setSocket(Socket *socket);
private:
	int id;
	string name;
	Socket *sock;
};

#endif

