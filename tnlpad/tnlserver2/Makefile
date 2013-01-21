OBJS=main.o channel.o client.o client_manager.o server.o packet.o
LIBS=
CFLAGS=-g -O2 -pipe
CXXFLAGS=$(CFLAGS)

PROG=tnlserver

all:$(OBJS)
	g++ -o $(PROG) $(OBJS) $(LIBS)

%o:%cpp
	g++ $(CXXFLAGS) -c $<

clean:
	rm $(OBJS) $(PROG)

