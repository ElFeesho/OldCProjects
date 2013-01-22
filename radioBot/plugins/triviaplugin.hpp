#ifndef __TRIVIA_PLUGIN_HPP__
#define __TRIVIA_PLUGIN_HPP__

#include "plugin.hpp"
#include <sqlite3.h>

class TriviaPlugin : public Plugin
{
	public:
		TriviaPlugin();
		~TriviaPlugin();
		
		void handle_msg(const string &sender, const string &channel, const string &msg);
		void handle_privmsg(const string &sender, const string &msg);
		void update();
		
		void plugin_loaded();
		void plugin_unloaded();
		
	private:
		sqlite3 *_db;				/// Questions database
		
		bool game_on;				/// Whether a game is in play or not
		int count_down;			/// The amount of seconds before a game is up
		unsigned long lstart; 	/// The timestamp of the beginning of the last / current game
};

#endif