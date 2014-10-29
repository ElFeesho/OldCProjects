OBJS=main.o gfx.o ops.o input.o sound.o
TOBJS=ops.o gfx.o input.o sound.o test.o
TPROG=tchip8
PROG=chip8
LIBS=`sdl-config --libs` -lm
CFLAGS=`sdl-config --cflags` -std=c99 -g

all:$(OBJS)
	gcc $(OBJS) -o $(PROG) $(LIBS)

test:$(TOBJS)
	gcc -o $(TPROG) $(TOBJS) $(LIBS)

%o:%c
	gcc -c $(CFLAGS) $<

flip:
	gcc -DFLIP main.c gfx.c ops.c -o chip8 $(LIBS) $(CFLAGS)

clean:
	rm -f $(OBJS) $(PROG)
