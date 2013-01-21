OBJS=main.o log.o network.o client.o channel.o
CFLAGS= -pipe -Wall -g
LIBS=-g
PROG=nlpserv

all:$(OBJS)
	g++ $(OBJS) -o $(PROG) $(LIBS)
	
%o:%cpp
	g++ -c $(CFLAGS) $< 

clean:
	rm -f $(OBJS) $(PROG)
