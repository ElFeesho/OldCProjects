#ifndef __FIRC_HPP__
#define __FIRC_HPP__

#include <sys/socket.h>
#include <netdb.h>
#include <string>

class firc
{
	public:
		firc(const char *server, short port);
		~firc();
		void set_raw_data(void (*on_raw_data_functor)(firc *instance, const char *data, int length));
		void set_privmsg(void (*on_data_functor)(firc *instance, const char *nick, const char *channel, const char *message));
		void set_command(void (*on_command_functor)(firc *instance, const char *nick, const char *channel, const char *command, const char *args), const char * delim);
		void set_close(void (*on_close_func)(firc *instance));

		void set_command_delim(const char *delim);

		void irc_connect();
		void disconnect();
		void send_login(char *user, char *realname, char *pass);
		void send_nick(char *new_nick);
		void send_data(const char *data);
		void send_join(const char *channel);
		void send_privmsg(const char *recip, const char *message);
		void send_action(const char *channel, const char *action);
		void send_part(const char *channel);
		void send_part(const char *channel, const char *reason);
		void send_quit();
		void send_quit(const char *reason);
		bool active();
	private:
		/* Socket Sensitive */
		int sock;

		/* Internal data */
		std::string server;
		short port;
		std::string nick;
		const char *command_delim;
		
		/* Callbacks */
		void (*on_raw_data)(firc *instance, const char *data, int length);
		void (*on_privmsg)(firc *instance, const char *nick, const char *channel, const char *message);
		void (*on_command)(firc *instance, const char *nick, const char *channel, const char *command, const char *arguments);
		void (*on_close)(firc *instance);
};

class firc_error
{
	public:
		firc_error(char *error);
		char *get_error();
	private:
		char *error_text;
};


#endif
