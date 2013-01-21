OBJS=main.o camera.o manipulate.o gui.o controls.o bayer.o network.o
CFLAGS=`pkg-config --cflags gtk+-2.0 libmimic` -O2 -pipe -march=nocona
LIBS=`pkg-config --libs gtk+-2.0 libmimic`
PROG=motion-test

all:$(OBJS)
	gcc $(OBJS) $(LIBS) -o $(PROG)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm $(OBJS) $(PROG) -f
