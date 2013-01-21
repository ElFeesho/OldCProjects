OBJS=main.o client.o channel.o packet_stream.o
CFLAGS=-mtune=core2 -Os
LIBS=
PROG=nlpserver

all:$(OBJS)
	g++ $(OBJS) -o $(PROG) $(LIBS) $(CFLAGS)

%o:%cpp
	g++ $(CFLAGS) $< -c

clean:
	rm -f $(OBJS) $(PROG)
