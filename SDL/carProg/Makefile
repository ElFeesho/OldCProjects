OBJS=main.o
CFLAGS=`sdl-config --cflags`
LIBS=`sdl-config --libs` -lSDL_image

PROG=carSong

all:$(OBJS)
	g++ $(OBJS) $(LIBS) -o $(PROG)

%o:%cpp
	g++ $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(PROG)
