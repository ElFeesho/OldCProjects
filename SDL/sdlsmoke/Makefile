OBJS=main.o pblib.o smoke.o
LIBS=`pkg-config sdl --libs` -lSDL_image -lSDL_gfx
CFLAGS=`pkg-config sdl --cflags`
PROG=sdlsmoke

all:$(OBJS)
	gcc -o $(PROG) $(OBJS) $(LIBS)

%o:%c
	gcc -c $(CFLAGS) $<

clean:
	rm $(OBJS) $(PROG) -f
