#include "channel.hpp"
#include "globals.hpp"
#include <string>


NP_Channel::NP_Channel(char *channel_name) : name(channel_name), owner(-1), default_voice(1), default_ink(1), topic(0), password(0)
{
	if(NP_Channel::chan_list==NULL)
		NP_Channel::chan_list = new std::vector<NP_Channel*>;
		
	NP_Channel::chan_list->push_back(this);
}

NP_Channel::~NP_Channel()
{
	
}

char *NP_Channel::get_name()
{
	return name;
}

int NP_Channel::get_owner()
{
	return owner;
}

int NP_Channel::get_id()
{
	return id;
}

bool NP_Channel::get_def_voice()
{
	return default_voice;
}

bool NP_Channel::get_def_ink()
{
	return default_ink;
}

char *NP_Channel::get_topic()
{
	return topic;
}

void NP_Channel::set_topic(char *new_topic)
{
	topic = new_topic;
}

void NP_Channel::set_def_voice(bool voice)
{
	default_voice = voice;
}

void NP_Channel::set_def_ink(bool ink)
{
	default_ink = ink;
}

void NP_Channel::set_password(char *new_password)
{
	password = new_password;
}

bool NP_Channel::check_password(char *pass)
{
	if(std::string(password) == std::string(pass))
		return 1;
	return 0;
}

std::vector<NP_Channel*> *NP_Channel::chan_list = NULL;
