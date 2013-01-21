OBJS=main.o menu.o game.o player.o netplayer.o enemy.o
CFLAGS=-I../swordfeesh/include -g `sdl-config --cflags`
CXXFLAGS=$(CFLAGS)
LIBS=-L../swordfeesh/lib -lswordfeesh `sdl-config --libs` -lSDL_mixer -lSDL_image -lGL -lGLU
PROG=invaders

all:$(OBJS)
	g++ -o $(PROG) $(OBJS) $(LIBS)

%o:%cpp
	g++ $(CXXFLAGS) -c $<

clean:
	rm -f $(PROG) $(OBJS)

