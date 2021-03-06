#include <iostream>
#include <string>

#include "net.hpp"

using namespace std;

int main(int argc, char **argv)
{
	cout << ":: Nettest Net unit test" << endl;
	
	cout << ":: 1. Net ctor" << endl;
	
	Net *net = new Net();
	
	cout << "Net *net = new Net(); = " << net << endl;
	
	cout << ":: 2. Connect to www.google.com:80" << endl;
	
	if(net->connect(string("www.google.com"), "80"))
	{
		cout << "Connected successfully" << endl;
	}
	else
	{
		cerr << "Failed to connect, terminating" << endl;
		return -1;
	}
	
	cout << ":: 3. Send GET / HTTP/1.1..." << endl;
	
	string get_request = string("GET / HTTP/1.1\r\nHost: www.google.com\r\n\r\n");
	
	net->send(get_request.c_str(), get_request.length());
	
	cout << ":: 4. Read response" << endl;
	string response;
	while ((response = net->read_line()).length()>0)
	{
		cout << "Response: " << response.c_str();
		if(response.compare("\r\n")==0)
			break;
	}
	
	cout << ":: 5. Shutdown connection" << endl;
	net->shutdown();
	
	cout << ":: 6. Delete net" << endl;
	delete net;
	
	cout << ":: End of nettest" << endl;
	return 0;
}