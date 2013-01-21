#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include "channel.hpp"
#include "packet.hpp"
#include <string>

using namespace std;

class Client
{
public:
	Client();
	~Client();

	void set_id(int nid);
	int get_id();

	void set_name(const char *name);
	string get_name();

	Packet *get_data();

	void set_chan(Channel *nchan);
	Channel *get_chan();
private:
	int id; // socket
	string name;
	Channel *chan;
};

#endif
