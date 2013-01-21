OBJS=main.o
CFLAGS=`pkg-config --cflags sdl`
LIBS=`pkg-config --libs sdl` -lwiiuse -lSDL_gfx
PROG=etchasketch

all:$(OBJS)
	gcc $(OBJS) $(LIBS) -o $(PROG)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(PROG)
