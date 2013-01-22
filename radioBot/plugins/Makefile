LIBS=-ldl -g
CFLAGS=-g
CXXFLAGS=$(CFLAGS)

all:
	g++ -shared -fPIC plugin.cpp irc.cpp net.cpp client.cpp scoreplugin.cpp -o scoreplugin
	g++ -shared -fPIC plugin.cpp irc.cpp net.cpp echoplugin.cpp -o echoplugin
	g++ -shared -fPIC plugin.cpp irc.cpp net.cpp pluginloader.cpp -o pluginloader
	g++ -shared -fPIC plugin.cpp irc.cpp net.cpp navplugin.cpp -o navplugin
	g++ -shared -fPIC plugin.cpp irc.cpp net.cpp triviaplugin.cpp -o triviaplugin -lsqlite3


	
	
