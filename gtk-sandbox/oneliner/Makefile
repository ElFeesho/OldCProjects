OBJS=main.o string_conv.o
LIBS=`pkg-config --libs gtk+-2.0`
CFLAGS=`pkg-config --cflags gtk+-2.0`
PROG=oneliner

all:$(OBJS)
	gcc -o $(PROG) $(OBJS) $(LIBS)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm $(PROG) $(OBJS) -f
