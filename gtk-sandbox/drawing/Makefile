LIBS=`pkg-config --libs gtk+-2.0`
CFLAGS=`pkg-config --cflags gtk+-2.0`
PROG=drawtest
OBJS=main.o

all:$(OBJS)
	gcc $(OBJS) -o $(PROG) $(LIBS)

%o:%c
	gcc -c $(CFLAGS) $< 

clean:
	rm -rf $(OBJS) $(PROG)
