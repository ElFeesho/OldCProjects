OBJS=main.o camera.o manipulate.o gui.o network.o
CFLAGS=`pkg-config --cflags gtk+-2.0 libmimic` -g
LIBS=`pkg-config --libs gtk+-2.0 libmimic` -g
PROG=motion-test

all:$(OBJS)
	gcc $(OBJS) $(LIBS) -o $(PROG)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm $(OBJS) $(PROG) -f
