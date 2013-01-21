#include "firc.hpp"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <signal.h>


firc *myCon = NULL;

template<typename To, typename From>
To lexical_cast(From f)
{
	To t;
	std::stringstream strm;
	strm << f; strm >> t;
	return t;
}

/*
	void on_raw_data(firc*, char*, int);
	THIS IS A CALLBACK FUNCTION THAT IS TIED TO A FIRC INSTANCE
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	This function is tied to an instance of firc, and it is done
	so by calling fircIns->set_raw_data(&on_raw_data);

	When firc gets data that it doesn't have a rule for, it is
	fed to this function. Numeric replies and actions go here
	(right now anyway).
*/
void on_raw_data(firc *ins, const char *data, int len)
{
	std::cout << data;
}

/*
	void on_command(firc*, const char*, const char*, const char*, const char*);
	THIS IS A CALLBACK FUNCTION THAT IS TIED TO A FIRC INSTANCE
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	This function is tied to an instance of firc, and it is done
	so by calling fircIns->set_command(&on_command);

	When the firc instance gets a privmsg (DOESN'T MEAN PRIVATE MESSAGE)
	this callback will be fired, with a pointer to itself, the nick
	that instagated the PRIVMSG, the channel it came from, and, the
	message.

*/
void on_command(firc *ins, const char *nick, const char *channel, const char *command, const char *args)
{
	if(channel!=NULL)
	{
		if(std::string(command)==std::string("dance"))
		{
			ins->send_privmsg(channel,"I'm dancing :D and perhaps romancing!");
		}
		else if(std::string(command)==std::string("rock"))
		{
			ins->send_privmsg(channel,"I'm like so totally rocking!");
		}
		else if(std::string(command)==std::string("quit"))
		{
			if(std::string(nick)==std::string("feesh"))
				ins->send_quit("Yes master!");
		}
		
		if(args!=NULL)
		{
			if(std::string(command)==std::string("changedelim"))
			{
				ins->set_command_delim(args);
			}
			if(std::string(command)==std::string("join"))
			{
				ins->send_join(args);
			}
		}
	}
}

/*
	void on_priv(firc*, const char*, const char*, const char*);
	THIS IS A CALLBACK FUNCTION THAT IS TIED TO A FIRC INSTANCE
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	This function is tied to an instance of firc, and it is done
	so by calling fircIns->set_privmsg(&on_priv);

	When the firc instance gets a privmsg (DOESN'T MEAN PRIVATE MESSAGE)
	this callback will be fired, with a pointer to itself, the nick
	that instagated the PRIVMSG, the channel it came from, and, the
	message.

*/
void on_priv(firc *ins, const char *nick, const char *channel, const char *message)
{
	static int chan_width = 0;
	static int nick_width = 0;
	if(std::string(nick).length()>nick_width)
		nick_width = std::string(nick).length();
	if(channel!=NULL)
	{
		if(std::string(channel).length()>chan_width)
			chan_width = std::string(channel).length();
		std::cout << "\x1b[34;1m" << std::setw(chan_width) << std::right << channel << "\x1b[33;0m " << std::setw(nick_width-std::string(nick).length()) << "<" << std::right << nick << "> " << message << std::endl;
	}
	else
		std::cout << std::setw(chan_width+nick_width-std::string(nick).length()+1) << "<" << nick << "> " << message << std::endl;
}

/*
	void on_close(firc*)
	THIS IS A CALLBACK FUNCTION THAT IS TIED TO A FIRC INSTANCE
	~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	This function is tied to an instance of firc, and it is done
	so by calling fircIns->set_close(&on_close);

	This function is fired when a firc instance is deleted 
	or destroyed. This is done by "delete fircIns;". This can be
	useful if you have open file streams or database handles that
	the bot may use and may need closing. However in this example
	it merely sends a quit message.
*/

void on_close(firc *con)
{
	con->send_quit("Hasta la victoria siempre");
}


/* Not part of firc, but nice to have 
	This function is tied to the SIGINT signal
	which is fired when a user hits CTRL+C on
	the program. In this handle my firc instance
	"myCon" is deleted, in turn, making on_close
	to be fired.
*/
void destroy_con(int signal)
{
	delete myCon;
}

int main(int argc, char **argv)
{
	if(argc!=4)
	{
		std::cout << "Usage: " << std::endl;
		std::cout << "\t" << argv[0] << " <server> <port> <channel>" << std::endl;
		return 1;
	}
	std::cout << "Connecting to " << argv[1] << " on port " << argv[2] << "..." << std::endl;
	//Creating my firc instance, setting the server and port
	myCon = new firc(argv[1],lexical_cast<int, char*>(argv[2]));
	
	//Now that I have my instance, I'm happy to 
	//tie up a signal to destroy it when necessary
	signal(SIGINT,destroy_con);

	//Setting the callbacks!
	try
	{
		//myCon->set_raw_data(&on_raw_data);
		myCon->set_privmsg(&on_priv);
		myCon->set_command(&on_command,"!");
		myCon->set_close(&on_close);
	}
	catch(firc_error *excep)
	{
		std::cout << "Couldn't set up callbacks: " << excep->get_error() << std::endl;
		return -1;
	}
	
	//Connect to the server
	try
	{
		myCon->irc_connect();
	}
	catch(firc_error *excep)
	{
		std::cout << "Failed connecting: " << excep->get_error() << std::endl;
		return -1;
	}
	std::cout << "Connected to server" << std::endl;

	//Send some vital details to the IRC server
	myCon->send_login("feebot","Feesh Bot","Feeshbot");
	myCon->send_nick("feeshbot");
	myCon->send_join(argv[3]);
	//The ever lasting loop- this has to be there, otherwise the instance
	//won't have the program flow. If anything bad happens or the bot disconnects
	//fircInst->active() will return 0, so the while loop will expire. Like before
	//if there are any file streams or database handles that need taking care of
	//now is the time to do it, as on_close isn't fired!
	while(myCon->active());
	return 0;
}

