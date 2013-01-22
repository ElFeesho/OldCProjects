#ifndef __SCORE_PLUGIN_HPP__
#define __SCORE_PLUGIN_HPP__

#include <vector>

#include "plugin.hpp"
#include "client.hpp"

using namespace std;

class ScorePlugin : public Plugin
{
	public:
		ScorePlugin();
		~ScorePlugin();
		
		void handle_privmsg(const string &sender, const string &msg);
		void handle_msg(const string &sender, const string &target, const string &msg);
		void handle_join(const string &who, const string &channel);
		void update();
		
		void plugin_loaded();
		void plugin_unloaded();
	private:
		vector<Client *> *clients;
};

#endif
