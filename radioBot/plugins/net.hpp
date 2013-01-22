#ifndef __NET_HPP__
#define __NET_HPP__

#include <string>

using std::string;

class Net
{
	public:
		Net();
		~Net();
		
		bool connect(const string &hostname, const string &port);
		void shutdown();
		
		bool can_read(int timeout = 1);
		
		string read(size_t len);
		string read_line(char delim = '\n');
		
		size_t send(const char *buffer, size_t len);
		size_t send_line(const char *buffer, char delim = '\0');
	private:
		int _sock;
};

#endif
