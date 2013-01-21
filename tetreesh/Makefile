OBJS=main.o menu.o bubble.o menusel.o game.o block.o piece.o
CFLAGS=`sdl-config --cflags` -I./include -g
CXXFLAGS=$(CFLAGS)
LIBS=-lswordfeesh `sdl-config --libs` -L./lib -lSDL_mixer -lSDL_image -lGL -lGLU
PROG=tetreesh

all:$(OBJS)
	g++ $(OBJS) $(LIBS) -o $(PROG)

%o:%cpp
	g++ $(CXXFLAGS) -c $<

clean:
	rm $(OBJS) $(PROG) -f

