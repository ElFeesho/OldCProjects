OBJS=main.o menu.o bubble.o menusel.o game.o block.o piece.o
CFLAGS=`sdl-config --cflags` -I../swordfeesh/include -g
CXXFLAGS=$(CFLAGS)
LIBS=-lswordfeesh `sdl-config --libs` -L../swordfeesh/lib -lSDL_mixer -lSDL_image -lGL -lGLU
PROG=feeshtris

all:$(OBJS)
	g++ -o $(PROG) $(OBJS) $(LIBS)

%o:%cpp
	g++ $(CXXFLAGS) -c $<

clean:
	rm $(OBJS) $(PROG) -f

