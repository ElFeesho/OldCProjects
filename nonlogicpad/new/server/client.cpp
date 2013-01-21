#include "client.hpp"
#include "channel.hpp"
#include <string>

NP_Client::NP_Client(int sock) : sock(sock), authed(0), active(0), nick(0), channel_id(-1), has_voice(0), has_ink(0), has_ops(0)
{
	//Empty!
}

NP_Client::~NP_Client()
{

}

int NP_Client::get_sock()
{
	return sock;
}

const char *NP_Client::get_nick()
{
	return nick;
}

int NP_Client::get_channel()
{
	return channel_id;
}

bool NP_Client::get_ink()
{
	return has_ink;
}

bool NP_Client::get_voice()
{
	return has_voice;
}

bool NP_Client::get_ops()
{
	return has_ops;
}

bool NP_Client::get_auth()
{
	return authed;
}

void NP_Client::set_nick(const char *new_nick)
{
	// Shouldn't need to copy the nick (should always be overwritten rather than editted)
	nick = std::string(new_nick).c_str(); 
}

void NP_Client::set_channel(char *channel_name, std::vector<NP_Channel*> channel_list)
{
	std::vector<NP_Channel*>::iterator channel_iter;
	
	for(channel_iter = NP_Channel::chan_list->begin();channel_iter!=NP_Channel::chan_list->end();channel_iter++)
	{
		if(std::string((*channel_iter)->get_name()) == std::string(channel_name))
		{
			//Found the channel, so set the channel id accordingly
			channel_id = (*channel_iter)->get_id();
			return;
		}
	}
	//Didn't find the channel, so lets add it!
	NP_Channel::chan_list->push_back(new NP_Channel(channel_name));
	//Since we just added a channel, it will be on the END of the vector, so end() will return it!
	channel_id = (*(NP_Channel::chan_list->end()))->get_id();
}

void NP_Client::set_voice(bool voice)
{
	has_voice = voice;
}

void NP_Client::set_ink(bool ink)
{
	has_ink = ink;
}

void NP_Client::set_ops(bool ops)
{
	has_ops = ops;
}
