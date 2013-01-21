#include "log.hpp"
#include <fstream>

using namespace std;

static fstream log_file;

extern bool log_open()
{
	if(!log_file.good())
	{
		log_file.open("logs/nlpad.log",ios::out);
		return log_file.good();
	}
	return true;
}

extern void log_write(char *text)
{
	if(!log_file.good())
	{
		log_open();
	}
	log_file << text;
}

extern void log_close()
{
	log_file.close();
}