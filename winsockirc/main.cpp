#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <cstdlib>
#include <winsock.h>

/*
	This example code is way over the top error checking wise (perhaps rightly so)
	for example, it is part of the TCP protocol that data MUST arrive and so
	send and recv, should NEVER return an amount other than the amount of data you
	requested, as it will wait and wait till it has it all, but I put checks in
	any way.
*/

using namespace std;

SOCKET create_socket(WSADATA *wsa_data)
{
	if(WSAStartup(MAKEWORD(1,1), wsa_data) != 0)
	{
		cerr << "Failed initialising Winsock API" << endl;
		return INVALID_SOCKET;
	}
	
	SOCKET return_sock = socket(AF_INET, SOCK_STREAM, 0); // socket(FAMILY, TYPE (tcp), FLAGS (no flags));
	
	if(return_sock == INVALID_SOCKET)
	{
		cerr << "Failed creating socket" << endl;
		return INVALID_SOCKET;
	}
	
	HOSTENT *host_info = gethostbyname("kornbluth.freenode.net"); // DNS resolution - convert text to IP :D
	if(host_info == NULL) // Failed to lookup host
	{
		cerr << "Couldn't obtain IP from hostname" << endl;
		return INVALID_SOCKET;
	}
	SOCKADDR_IN sock_addr; // Host information used whilst connecting
	sock_addr.sin_port = htons(6667); // Set the port we're going to connect to 6667 
	// htons converts any endian to BIG endian (or network endian which I believe is big endian)
	sock_addr.sin_family = AF_INET;
	sock_addr.sin_addr = *((LPIN_ADDR)*host_info->h_addr_list);
	/* Previous line sets the address we're going to connect to, the line looks pretty mean
		but basically: it changes the first available ip in the host_info (since one dns name
		can point to multiple ip's) into an LPIN_ADDR structure, which is then set to sock_addr.sin_addr
	*/
	
	int result = 0;
	if ((result = connect(return_sock, (LPSOCKADDR)&sock_addr, sizeof(SOCKADDR_IN))) == SOCKET_ERROR)
	{
		cerr << "Failed to connect to server - ERROR Code: " << result << endl;
	   return INVALID_SOCKET;
	}
	
	return return_sock;
}

int send_login(SOCKET handle, string nick, string user, string pass, string realname)
{
	string user_line("USER ");
	user_line.append(user).append(" Localhost Remotehost :").append(realname).append("\r\n");
	string pass_line("PASS ");
	pass_line.append(pass).append("\r\n");
	string nick_line("NICK ");
	nick_line.append(nick).append("\r\n");
	
	//send(SOCKET, LPVOID BUFFER, LONG SIZE, FLAGS)
	
	if(send(handle, user_line.c_str(), user_line.length(),0) != user_line.length())
	{
		cerr << "Failed sending `USER' line" << endl;
		return -1;
	}
	
	if(send(handle, pass_line.c_str(), pass_line.length(), 0) != pass_line.length())
	{
		cerr << "Failed sending `PASS` line" << endl;
		return -1;
	}
	
	if(send(handle, nick_line.c_str(), nick_line.length(),0) != nick_line.length())
	{
		cerr << "Failed sending `NICK' line" << endl;
		return -1;
	}
	
	return 0;
}

int send_join(SOCKET handle, string channel)
{
	string join_line("JOIN ");
	join_line.append(channel).append("\r\n");
	
	if(send(handle, join_line.c_str(), join_line.length(),0) != join_line.length())
	{
		cerr << "Failed sending `JOIN' line" << endl;
		return -1;
	}
	return 0;
}

int receive_data(SOCKET handle, string *buffer)
{
	//recv(SOCKET, LPVOID BUFFER, LONG SIZE, FLAGS)
	char *garb_buffer = new char[1500]; //1500 usually == MTU (Maximum transmission Unit)
	memset(garb_buffer,'\0',1500);
	int amount = 0; // Will hold how much data there is to read
	
	amount = recv(handle,garb_buffer,1500,MSG_PEEK);

	// MSG_PEEK Flag doesn't remove any data in the socket so it's safe to call recv to get it again
	char *main_buf = new char[amount];
	memset(main_buf,'\0',amount);
	if(recv(handle,main_buf,amount,0)!=amount)
	{
		cerr << "Didn't receive all the data" << endl;
		return -1;
	}
	main_buf[amount-2] = '\0';
	(*buffer) = main_buf;
	delete main_buf;
	delete garb_buffer;
	return amount;
}

int send_privmsg(SOCKET handle, string recip, string message)
{
	string formatted_pm("PRIVMSG ");
	formatted_pm.append(recip).append(" :").append(message).append("\r\n");
	
	if(send(handle, formatted_pm.c_str(), formatted_pm.length(), 0) != formatted_pm.length())
	{
		cerr << "Didn't send all of pm data" << endl;
		return -1;
	}

	return 0;
}

int send_quit(SOCKET handle, string message)
{
	string formatted_pm("QUIT ");
	formatted_pm.append(message).append("\r\n");
	
	if(send(handle, formatted_pm.c_str(), formatted_pm.length(), 0) != formatted_pm.length())
	{
		cerr << "Didn't send all of quit message" << endl;
		return -1;
	}

	return 0;
}

int main(int argc, char **argv)
{
	cout << "Winsock IRC" << endl;

	srand(time(NULL));

	WSADATA wsa_data; // I can only imagine this holds a winsock internal reference or something
	SOCKET irc_handle = create_socket(&wsa_data);
	
	if(irc_handle == INVALID_SOCKET)
	{
		cerr << "Exitting from previous errors" << endl;
		return 1;
	}
	
	cout << "Connected" << endl;
	
	if(send_login(irc_handle,"Feeshwbot","feeuser","zuzu","edward woo")!=0)
	{
		cerr << "Failed to send login" << endl;
		return 1;
	}
	
	if(send_join(irc_handle,"##test##")!=0)
	{
		cerr << "Failed to join channel" << endl;
		return 1;
	}
	
	for(;;)
	{	
		string irc_buffer;
		if(receive_data(irc_handle, &irc_buffer)!=-1)
		{
			//Place if's and string tokenizer code here to detect which user said what and so on.
			cout << irc_buffer.c_str() << endl;
			if(irc_buffer.find("PRIVMSG")!=string::npos)
			{
				string nick, channel, message;
				nick = irc_buffer.substr(1,irc_buffer.find("!")-1);
				if(irc_buffer.find("#")!=string::npos && irc_buffer.find("#")<irc_buffer.find(" :"))
					channel = irc_buffer.substr(irc_buffer.find("#"),irc_buffer.find(" :")-irc_buffer.find("#"));
				else
					channel = "";

				message = irc_buffer.substr(irc_buffer.find(" :")+2,(irc_buffer.length())-(irc_buffer.find(" :")+2));			
				cout << "NICK : " << nick.c_str() << endl << "MESSAGE: " << message.c_str() << endl << "CHANNEL : " << channel.c_str() << endl;
				if(nick == "feesh")
				{
					if(message == "COOKIES")
					{
						if(send_privmsg(irc_handle, "##test##","YOU HAVE COOKIES!!!?")!=0)
						{
							cerr << "Failed sending private message" << endl;
							return 1;
						}
					}
					if(message == "RAND")
					{
						stringstream conv; 
						conv << rand()%1000;
						char *output = new char[5];
						memset(output,'\0',5);
						conv >> output;
						send_privmsg(irc_handle, "##test##",output);
					}
					if(message == "QUIT")
					{
						if(send_quit(irc_handle, "Goodbye")!=0)
						{
							cerr << "Failed sending quit" << endl;
							return 1;
						}
						break;
					}
				}
			}
		}
	}
	closesocket(irc_handle);
   return 0;
}
