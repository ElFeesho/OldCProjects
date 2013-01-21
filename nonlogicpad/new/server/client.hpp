#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <vector>
#include "channel.hpp"

class NP_Client
{
	public:
		NP_Client(int sock);
		~NP_Client();
		int get_sock();
		const char *get_nick();
		int get_channel();
		bool get_ops();
		bool get_ink();
		bool get_voice();
		bool get_auth();
		
		void set_nick(const char *new_nick);
		void set_channel(char *channel_name, std::vector<NP_Channel*> channel_list);
		void set_voice(bool voice);
		void set_ink(bool ink);
		void set_ops(bool ops);
		void set_authed(bool auth);
		
	private:
		int sock; //Id pretty much
		bool authed;
		bool active;
		const char *nick;
		int channel_id;
		bool has_voice;
		bool has_ink;
		bool has_ops;
};

#endif
