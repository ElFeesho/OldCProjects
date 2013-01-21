OBJS=main.o camera.o manipulate.o gui.o network.o bitmap.o
CFLAGS=`pkg-config --cflags libmimic` -g
LIBS=`pkg-config --libs libmimic` -lm -g
PROG=motion-test

all:$(OBJS)
	gcc $(OBJS) $(LIBS) -o $(PROG)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm $(OBJS) $(PROG) -f
