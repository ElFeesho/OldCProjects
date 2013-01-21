OBJS=main.o log.o network.o socket.o client.o
CFLAGS=-O3 -pipe -Wall
LIBS=
PROG=nlpserv

all:$(OBJS)
	gcc $(OBJS) -o $(PROG) $(LIBS)
	
%o:%c
	gcc -c $(CFLAGS) $< 

clean:
	rm -f $(OBJS) $(PROG)
