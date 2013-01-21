OBJS=main.o network.o stringops.o core_ssl.o gui.o error.o mwin.o conversation.o globals.o
CFLAGS=`pkg-config --cflags gtk+-2.0 gnet-2.0`
LIBS=`pkg-config --libs gtk+-2.0 gnet-2.0 openssl`
PROG=fmsn

all:$(OBJS)
	gcc -o $(PROG) $(OBJS) $(LIBS)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(PROG)
