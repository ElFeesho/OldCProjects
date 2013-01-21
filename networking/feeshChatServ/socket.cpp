#include "socket.hpp"

#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>

Socket::Socket()
{
	__sock = socket(PF_INET, SOCK_STREAM, 0);
	int yes = 1;
	setsockopt(__sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int));
}

Socket::Socket(int sock)
{
	__sock = sock;
}

Socket::~Socket()
{
	shutdown();
}

void Socket::connect(string host, int pNum)
{
	struct sockaddr_in dst;
	struct hostent *hostEnt;
	hostEnt = gethostbyname(host.c_str());

	if(!hostEnt)
	{
		cerr << "SOCKET ERROR: Couldn't resolve host: " << host.c_str() << endl;
		return;
	}

	dst.sin_family = PF_INET;
	dst.sin_port = htons(pNum);
	dst.sin_addr = *((struct in_addr*)hostEnt->h_addr);

	if(::connect(__sock, (struct sockaddr*)&dst, sizeof(struct sockaddr_in))!=0)
		cerr << "SOCKET ERROR: Couldn't connect to " << host.c_str() << endl;
}

bool Socket::listen(unsigned short pNum)
{
	struct sockaddr_in dst;
	dst.sin_family = PF_INET;
	dst.sin_port = htons(pNum);
	dst.sin_addr.s_addr = INADDR_ANY;
	if(bind(__sock, (struct sockaddr*)&dst, sizeof(struct sockaddr_in))!=0)
	{
		cerr << "SOCKET ERROR: Couldn't bind local address." << endl; 
		return false;
	}
	if(::listen(__sock, 255)!=0)
	{
		cerr << "SOCKET ERROR: Couldn't start listening on port " << pNum << endl;
		return false;
	}

	return true;
}

Socket *Socket::accept()
{
	return new Socket(::accept(__sock, NULL, NULL));
}

int Socket::getSocket()
{
	return __sock;
}

void Socket::setBlocking(bool block)
{
	int opts = fcntl(__sock,F_GETFL,0);
	if(!block)
	{
		if((opts&O_NONBLOCK) == O_NONBLOCK)
   		opts ^= O_NONBLOCK;
		else
			opts |= O_NONBLOCK;
	}
   if(fcntl(__sock,F_SETFL,opts)==0)
   {
      return;
   }
	else
	{
		cerr << "SOCKET ERROR: Failed setting nonblock" << endl;   
      return;
   }
}

void Socket::shutdown()
{
	::shutdown(__sock,SHUT_RDWR);
	close(__sock);
}

int Socket::send(char data)
{
	return ::send(__sock, &data, 1, 0);
}

int Socket::send(const char *data, int len)
{
	return ::send(__sock, data, len, 0);
}

char Socket::recv()
{
	char ret = 0;
	::recv(__sock, &ret, 1, 0);
	return ret;
}

char *Socket::recv(int len)
{
	char *return_val = new char[len];

	::recv(__sock, return_val, len, 0);

	return return_val;
}
