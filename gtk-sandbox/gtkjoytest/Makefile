OBJS=main.o joy.o
CFLAGS=`pkg-config --cflags gtk+-2.0` -g
LIBS=`pkg-config --libs gtk+-2.0 gthread-2.0`
PROG=gtkjoytest

all:$(OBJS)
	gcc $(OBJS) -o $(PROG) $(LIBS)

%o:%c
	gcc $(CFLAGS) -c $< 

clean:
	rm -rf *.o $(PROG)
