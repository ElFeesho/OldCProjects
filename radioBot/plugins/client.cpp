#include "client.hpp"

Client::Client(const string &nick)
{
	this->nick =new String(nick);
	score = 100;
}

Client::~Client()
{
	delete nick;
}

void Client::increment()
{
	score++;
}

void Client::decrement()
{
	score--;
}

string *Client::getNick()
{
	return nick;
}

int Client::getScore()
{
	return score;
}
