#ifndef __CHANNEL_H__
#define __CHANNEL_H__

#include <vector>

class NP_Channel
{
	public:
		NP_Channel(char *channel_name);
		~NP_Channel();
		char *get_name();
		int get_id();
		int get_owner();
		bool get_def_voice();
		bool get_def_ink();
		char *get_topic();
		bool check_password(char *pass);
		
		void set_topic(char *new_topic);
		void set_def_voice(bool voice);
		void set_def_ink(bool ink);
		void set_password(char *new_password);
		static std::vector<NP_Channel*> *chan_list;
	private:
		char *name;
		int id;
		int owner;
		bool default_voice;
		bool default_ink;
		char *topic;
		char *password;
	
};

#endif
