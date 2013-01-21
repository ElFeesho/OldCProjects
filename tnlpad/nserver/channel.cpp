#include <iostream>
#include <cstdlib>
#include <cstring>
#include "channel.h"

Channel::Channel(char *nname, int id) : id(id), default_attribs(3), channel_attribs(0)
{
	strncpy(name,nname, 32);
	memset(topic,'\0',128);
	memset(pass,'\0',32);
	client_list = new std::vector<Client *>;
	ops = new std::vector<Client *>;
}

Channel::~Channel()
{

}

void Channel::set_channel_attribs(int nattrs)
{
	channel_attribs = nattrs;
}

void Channel::set_default_attribs(int ndefs)
{
	default_attribs = ndefs;
}

void Channel::set_topic(char *ntopic)
{
	strncpy(topic, ntopic, 128);
}

void Channel::set_pass(char *npass)
{
	strncpy(pass, npass, 32);
}
	
int Channel::get_channel_attribs() const
{
	return channel_attribs;
}

int Channel::get_default_attribs() const
{
	return default_attribs;
}

char *Channel::get_name()
{
	return name;
}

char *Channel::get_pass()
{
	return pass;
}
	
char *Channel::get_topic()
{
	return topic;
}

bool Channel::is_op(Client *client)
{
	if(ops->size()==0)
		return false;
	for(std::vector<Client*>::iterator iter = ops->begin();iter!=ops->end();iter++)
	{
		if((*iter)==client)
		{
			return true;
		}
	}
	return false;
}

std::vector<Client*> *Channel::get_client_list()
{
	return client_list;
}

void Channel::add_op(Client *client)
{
	ops->push_back(client);
}

void Channel::remove_op(Client *client)
{
	if(!ops)
		return;
	for(std::vector<Client*>::iterator iter = ops->begin();iter!=ops->end();iter++)
	{
		if((*iter)==client)
		{
			ops->erase(iter);
			return;
		}
	}
	std::cout << "DEBUG: Client was not found or is not an op" << std::endl;
}
