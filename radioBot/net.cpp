#include "net.hpp"

#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <cstring>
#include <sys/types.h>
#include <unistd.h>

Net::Net()
{
}

Net::~Net()
{
	if(_sock>0)
		shutdown();
}

bool Net::connect(const string &hostname, const string &port)
{
	struct addrinfo hints, *res;
	
	// first, load up address structs with getaddrinfo():
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	getaddrinfo(hostname.c_str(), port.c_str(), &hints, &res);

	_sock = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

	return ::connect(_sock, res->ai_addr, res->ai_addrlen)==0;
}

void Net::shutdown()
{
	if(_sock>0) // Only shutdown if it's open
		::shutdown(_sock, SHUT_RDWR);
}

bool Net::can_read(int timeout)
{
	if(_sock<=0)
		return false;
	
	struct timeval tv;
	fd_set readfds;

	tv.tv_sec = timeout;
	tv.tv_usec = 0;

	FD_ZERO(&readfds);
	FD_SET(_sock, &readfds);

	// don't care about writefds and exceptfds:
	select(_sock+1, &readfds, NULL, NULL, &tv);
	if (FD_ISSET(_sock, &readfds))
		return true;
	else
		return false;
}

string Net::read(size_t len)
{
	string ret_val = "";
	
	char *buf = new char[len];
	recv(_sock, buf, len, 0);
	
	ret_val.append(buf);
	
	delete buf;
	
	return ret_val;
}

string Net::read_line(char delim)
{
	string ret_val = "";
	char buf = delim-1; // just make sure we don't set the buf to the delim
	while(buf!=delim)
	{
		recv(_sock, &buf, 1, 0);
		ret_val.push_back(buf);
	}
	
	return ret_val;
}
		
size_t Net::send(const char *buffer, size_t len)
{
	// easy peasy...
	return ::send(_sock, buffer, len, 0);
}

size_t Net::send_line(const char *buffer, char delim)
{
	size_t len = 1;
	
	while(buffer[len] != delim)
		len++;
	
	return send(buffer, len);
}