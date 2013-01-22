OBJS=main.o net.o irc.o plugin.o
LIBS=-ldl -g
CFLAGS=-g
CXXFLAGS=$(CFLAGS)
PROG=radioBot
all:$(OBJS)
	g++ -o $(PROG) $(OBJS) $(LIBS)

%o:%cpp
	g++ $(CXXFLAGS) -c $<

nettest:nettest.o net.o
	g++ nettest.cpp net.cpp -o nettest

plugins:echoplugin.cpp
	g++ -shared -fPIC plugin.cpp irc.cpp net.cpp echoplugin.cpp -o echoplugin
	g++ -shared -fPIC plugin.cpp irc.cpp net.cpp pluginloader.cpp -o pluginloader
	g++ -shared -fPIC plugin.cpp irc.cpp net.cpp navplugin.cpp -o navplugin
	g++ -shared -fPIC plugin.cpp irc.cpp net.cpp triviaplugin.cpp -o triviaplugin -lsqlite3

	
clean:
	rm -f $(OBJS) $(PROG)
