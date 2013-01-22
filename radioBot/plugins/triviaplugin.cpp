#include "triviaplugin.hpp"

#include <sqlite3.h>

#include <sstream>

using std::stringstream;

template<typename Target, typename Source>
	Target lexical_cast(const Source& arg)
	{
		Target res;
		stringstream conv;
		conv << arg;
		conv >> res;
		return res;
	}


TriviaPlugin::TriviaPlugin() : _db(NULL), game_on(false), count_down(30)
{
	name = "Trivia Plugin";
	// Open the database, and create it if necessary
	sqlite3_open("trivia.db",&_db);
	if(_db == NULL)
	{
		return;
	}
	
	sqlite3_exec(_db,"CREATE TABLE IF NOT EXISTS \"question\" (\"qid\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL , \"question\" VARCHAR NOT NULL  UNIQUE , \"answer\" VARCHAR NOT NULL , \"type\" INTEGER NOT NULL )", NULL,NULL,NULL);
	sqlite3_exec(_db,"CREATE TABLE IF NOT EXISTS \"score\" (\"sid\" INTEGER PRIMARY KEY  AUTOINCREMENT  NOT NULL , \"user\" VARCHAR NOT NULL  UNIQUE , \"nick\" VARCHAR NOT NULL , \"score\" INTEGER NOT NULL )", NULL, NULL, NULL);
	sqlite3_close(_db);
}

TriviaPlugin::~TriviaPlugin()
{
}

void TriviaPlugin::handle_msg(const string &sender, const string &channel, const string &msg)
{
}

void TriviaPlugin::handle_privmsg(const string &sender, const string &msg)
{
	if(sender.compare("feesh") != 0)
	{
		return;
	}
	if(msg.find("!addq ") == 0) // Adding a question
	{
		// question format should be 
		// !addq [question]:[answer]{:[type]}}
		
		
	}
}

void TriviaPlugin::update()
{
}

	
void TriviaPlugin::plugin_loaded()
{
}

void TriviaPlugin::plugin_unloaded()
{
}


extern "C"
{
	Plugin *get_plugin()
	{
		return new TriviaPlugin;
	}
	
	void destroy_plugin(Plugin *plug)
	{
		delete plug;
	}
}