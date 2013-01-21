OBJS=main.o network.o gui.o
CFLAGS=`pkg-config --cflags gtk+-2.0 gnet-2.0`
LIBS=`pkg-config --libs gtk+-2.0 gnet-2.0`
PROG=udsplat

all:$(OBJS)
	gcc $(OBJS) -o $(PROG) $(LIBS)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(PROG)
	
