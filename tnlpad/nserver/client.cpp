#include "client.h"

#include <sys/socket.h>
#include <unistd.h>

#include <cstring>

Client::Client() : id(0), logged_in(0), status(0), attribs(0), current_channel(0)
{
	
}

Client::~Client()
{
	//close socket
	close(id);
}

void Client::set_id(int nid)
{
	id = nid;
}

void Client::set_logged_in()
{
	logged_in = 1;
}

void Client::set_status(int nstatus)
{
	status = nstatus;	
}

void Client::set_attribs(int nattribs)
{
	attribs = nattribs;
}

void Client::set_nick(char *nnick)
{
	strcpy(nick, nnick);
}

void Client::set_name(char *nname)
{
	strcpy(name,nname);
}

void Client::set_website(char *nwebsite)
{
	strcpy(website,nwebsite);
}

void Client::set_email(char *nemail)
{
	strcpy(email, nemail);
}

void Client::set_channel(void *ncurrent_channel)
{
	current_channel = ncurrent_channel;
}
// Deleting clients happens at the beginning of the loop
void Client::set_delete()
{
	to_delete = true;
}

int Client::get_id() const
{
	return id;
}

int Client::get_status() const
{
	return status;
}

int Client::get_attribs() const
{
	return attribs;
}

bool Client::get_logged_in() const
{
	return logged_in;
}

char *Client::get_nick()
{
	return nick;
}

char *Client::get_name()
{
	return name;
}

char *Client::get_website()
{
	return website;
}

char *Client::get_email()
{
	return email;
}

void *Client::get_channel()
{
	return current_channel;
}

bool Client::get_delete()
{
	return to_delete;
}
