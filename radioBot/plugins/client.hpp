#ifndef __CLIENT_HPP__
#define __CLIENT_HPP__

#include <string>

using namespace std;

class Client
{
	private:
		string *nick;
		int score;
	public:
		Client(const string &nick);
		~Client();

		void increment();
		void decrement();

		string *getNick();
		int getScore();		
};

#endif
