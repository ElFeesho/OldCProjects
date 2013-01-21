#include "client.hpp"

Client::Client()
{
	id = -1;
}

Client::Client(Socket *socket)
{
	id = socket->getSocket();
	sock = socket;
}

Client::~Client()
{
	delete sock;
}

int Client::getID()
{
	return id;
}

void Client::setID(int nid)
{
	id = nid;
}

string Client::getName()
{
	return name;
}

void Client::setName(string nName)
{
	name = nName;
}

Socket *Client::getSocket()
{
	return sock;
}

void Client::setSocket(Socket *socket)
{
	sock = socket;
	setID(socket->getSocket());
}
