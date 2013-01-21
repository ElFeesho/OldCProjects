OBJS=main.o menu.o game.o roflcoptah.o bg.o wtfbomb.o omgissile.o 
CFLAGS=`sdl-config --cflags` -I./include
CXXFLAGS=$(CFLAGS)
LIBS=`sdl-config --libs` -L./lib -lSDL_image -lSDL_mixer -lGLU -lswordfeesh

all:$(OBJS)
	g++ $(OBJS) $(LIBS) -o roflcoptah

%o:%cpp
	g++ $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) roflcoptah
