OBJS=main.o gfx.o ops.o input.o
PROG=chip8
LIBS=`sdl-config --libs`
CFLAGS=`sdl-config --cflags` -std=c99 -g

all:$(OBJS)
	gcc $(OBJS) -o $(PROG) $(LIBS)

%o:%c
	gcc -c $(CFLAGS) $<

flip:
	gcc -DFLIP main.c gfx.c ops.c -o chip8 $(LIBS) $(CFLAGS)

clean:
	rm -f $(OBJS) $(PROG)
