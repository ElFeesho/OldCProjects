OBJS=main.o draw.o
LIBS=`pkg-config --libs gtk+-2.0`
CFLAGS=`pkg-config --cflags gtk+-2.0`

PROG=flashish

all:$(OBJS)
	gcc -o $(PROG) $(OBJS) $(LIBS)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(PROG)

