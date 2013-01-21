#include <iostream>
#include <vector>
#include <cstdio>

#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>

#include "client.hpp"
#include "socket.hpp"

using namespace std;

Socket hostSock;
vector<Client *> clients;
fd_set client_sockets;

void handleIncoming();
int updateFDSet();
void handleClient(Client *client);

int main(int argc, char **argv)
{
	// Create and clear an fd_set - used with select for asynchronous socket IO
	
	FD_ZERO(&client_sockets);

	hostSock.setBlocking(false);
	
	if(!hostSock.listen(12345))
		return 0;
	
	for(;;) // Main loop
	{
		int ret = updateFDSet();

		if(ret==0) // No sockets had any data to be read
			continue;

		if(FD_ISSET(hostSock.getSocket(), &client_sockets))
		{
			handleIncoming();
		}
		for(vector<Client*>::iterator it = clients.begin(); it!=clients.end();)
		{
			Client *client = (*it);
			if(FD_ISSET(client->getID(), &client_sockets)) // This client has data!
			{
				handleClient(client);
			}
			it++;
		}
	}

	return 0;
}

int updateFDSet()
{
	int highSocket = hostSock.getSocket()+1;
	FD_ZERO(&client_sockets);
	FD_SET(hostSock.getSocket(), &client_sockets);
	for(vector<Client*>::iterator it = clients.begin(); it!=clients.end();it++)
	{
		// Add all client sockets to the FD set, so select can poll them
		FD_SET((*it)->getID(),&client_sockets);
		if((*it)->getID()>=highSocket)
			highSocket = (*it)->getID()+1;
	}
	return select(highSocket, &client_sockets, NULL, NULL, NULL);
}

void handleIncoming()
{
	// There is an incoming connection!! Answer it!
	Client *nclient = new Client(hostSock.accept());

	cout << "Got a connection!" << endl;

	Socket *cSock = nclient->getSocket();
	
	cout << "SOCK ID: " << nclient->getID() << endl;

	char header[2] = { 0 };
	header[0] = cSock->recv();
	
	if(header[0]!=0x01) // Bad client, didn't send login header
	{
		cerr << "Bad client" << endl;
		delete nclient;
		return;
	}

	string username;
	string password;
	header[0] = cSock->recv(); // Get the username
	while(header[0]!=0x02)
	{
		username.append(header);
		header[0] = cSock->recv();
	}

	header[0] = cSock->recv(); // Get the password
	while(header[0]!=0x03)
	{
		password.append(header);
		header[0] = cSock->recv();
	}

	nclient->setName(username);
	for(vector<Client *>::iterator it = clients.begin(); it!=clients.end();it++)
	{
		Socket *clSock = (*it)->getSocket();
		clSock->send('\x04');
		clSock->send((char)nclient->getID());
		clSock->send(nclient->getName().c_str(), nclient->getName().length());
		clSock->send('\x05');
		printf("Sending to %s, client %s\n", (*it)->getName().c_str(), nclient->getName().c_str());
	}

	for(vector<Client *>::iterator it = clients.begin(); it!=clients.end();it++)
	{	
		Socket *clSock = nclient->getSocket();
		clSock->send('\x04');
		clSock->send((char)(*it)->getID());
		clSock->send((*it)->getName().c_str(), (*it)->getName().length());
		clSock->send('\x05');
		printf("Sending to %s, client %s\n", nclient->getName().c_str(),(*it)->getName().c_str());
	}

	clients.push_back(nclient);
	cout << "Got a new client " << nclient->getName().c_str()  << endl;
	cSock->setBlocking(false);
}

void handleClient(Client *client)
{
	char header[2] = { 0 };
	Socket *cSock = client->getSocket();
	header[0] = cSock->recv();
	if(header[0] == 0x04) // Name change
	{
		header[0] = cSock->recv();
		string username;
		while(header[0]!=0x05)
		{
			username.append(header);
			header[0] = cSock->recv();
		}
		client->setName(username);
		// UPDATE ALL CLIENTS OF NEW CHANGE!
		for(vector<Client *>::iterator it = clients.begin(); it!=clients.end();it++)
		{
			if((*it)->getID()!=client->getID())
			{
				Socket *clSock = (*it)->getSocket();
				clSock->send('\x05');
				clSock->send((char)client->getID());
				clSock->send(client->getName().c_str(), client->getName().length());
				clSock->send('\x06');
			}
		}
		printf("Client changed name to: %s\n", username.c_str());
		return;		
	}
	else if(header[0] == 0x06) // Message
	{
		header[0] = cSock->recv();
		int id = header[0];
		string message;
		header[0] = cSock->recv();
		while(header[0]!=0x07)
		{
			message.append(header);
			header[0] = cSock->recv();
		}
		printf("RECEIVED MESSAGE FROM: %s for %d: %s\n",client->getName().c_str(), id, message.c_str());
		// Send message to client with the id of ID
		for(vector<Client *>::iterator it = clients.begin(); it!=clients.end();it++)
		{
			if((*it)->getID()==id)
			{
				Socket *clSock = (*it)->getSocket();
				clSock->send('\x06');
				clSock->send((char)client->getID());
				clSock->send(message.c_str(), message.length());
				clSock->send('\x07');
			}
		}
		return;		
	}

}
