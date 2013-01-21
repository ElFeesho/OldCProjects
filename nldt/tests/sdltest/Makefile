OBJS=main.o
CFLAGS=
LIBS=-lSDL -lSDL_gfx -lm
PROG=sdltest

all:$(OBJS)
	gcc $(LIBS) -o $(PROG) $(OBJS)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(PROG)
