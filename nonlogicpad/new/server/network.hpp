#ifndef __NETWORK_H__
#define __NETWORK_H__

#include <sys/socket.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/select.h>
#include <vector>
#include "client.hpp"
#include "channel.hpp"

/*
 * NP_Network:
 *  
 * This class holds all the network based details, i.e. anything to do
 * with network should/WILL be in here! From new connections to incoming
 * data.
 */

class NP_Network
{
	public:
		NP_Network();
		NP_Network(short port);
		~NP_Network();
		
		// For use when reading data in from clients
		int get_sock();
		unsigned char get_header(NP_Client *client);
		unsigned char *get_data(NP_Client *client, int *length);
		fd_set *get_sock_set();
		std::vector<NP_Client*> *get_clients();
		
		void accept_clients();
		int update_socks();
	private:
		void non_block(int sock);
		int listen_sock;
		fd_set *sock_set;
		std::vector<NP_Client*> *client_list;
		
};

#endif
