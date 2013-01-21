OBJS=main.o gtkccanvas.o
LIBS=`pkg-config --libs gtk+-2.0`
CFLAGS=`pkg-config --cflags gtk+-2.0` -c -pipe

all:$(OBJS)
	gcc $(OBJS) $(LIBS) -o ccanvas

%o:%c
	gcc $(CFLAGS) $<

clean:
	rm -f ccanvas $(OBJS)
