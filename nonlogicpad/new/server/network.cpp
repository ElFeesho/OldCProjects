#include <stdlib.h>
#include <memory.h>
#include "network.hpp"
#include "log.hpp"

NP_Network::NP_Network()
{
	client_list = new std::vector<NP_Client*>;
	sock_set = new fd_set;
}

NP_Network::NP_Network(short port)
{
	client_list = new std::vector<NP_Client*>;
	sock_set = new fd_set;
	
	listen_sock = socket(AF_INET, SOCK_STREAM,0);
	struct sockaddr_in network_host;
	
	network_host.sin_family = AF_INET;
	network_host.sin_port = htons(port);
	network_host.sin_addr.s_addr = INADDR_ANY;
	memset(network_host.sin_zero,0x0,8);
	if(bind(listen_sock,(struct sockaddr*)&network_host,sizeof(struct sockaddr_in))==-1)
		exit(1);
	non_block(listen_sock);
	
	listen(listen_sock,255);
}

NP_Network::~NP_Network()
{

}

int NP_Network::get_sock()
{
	return listen_sock;
}

fd_set *NP_Network::get_sock_set()
{
	return sock_set;
}

std::vector<NP_Client*> *NP_Network::get_clients()
{
	return client_list;
}

void NP_Network::non_block(int sock)
{
	int opts = fcntl(sock,F_GETFL,0);
	opts |= O_NONBLOCK;
	if(fcntl(sock,F_SETFL,opts)==0)
	{
		return;
	}else{
		log_write("Nonblocking failed\n");
		exit(1);
		return;
	}
}


void NP_Network::accept_clients()
{
	struct sockaddr_in addr;
	unsigned int size = sizeof(struct sockaddr_in);
	int new_sock = accept(listen_sock,(struct sockaddr*)&addr,&size);
	if(new_sock>0)
	{
		non_block(new_sock);
		client_list->push_back(new NP_Client(new_sock));
	}
	else
	{
		log_write("Failed to accept connection");
	}
}

int NP_Network::update_socks()
{
	int high_sock = 0;
	FD_ZERO(sock_set);
	
	FD_SET(listen_sock,sock_set);
	high_sock = listen_sock+1;
	for(std::vector<NP_Client*>::iterator it = client_list->begin();it!=client_list->end();it++)
	{
		FD_SET((*it)->get_sock(),sock_set);
		if((*it)->get_sock() > high_sock)
			high_sock = (*it)->get_sock()+1;
	}
	return high_sock;
}


