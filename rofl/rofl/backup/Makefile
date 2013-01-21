OBJS=main.o engine.o entity.o menu.o game.o sprite.o roflcoptah.o scene.o entitylist.o bg.o image.o
CFLAGS=`sdl-config --cflags` -g
CXXFLAGS=$(CFLAGS)
LIBS=`sdl-config --libs` -lSDL_image -lSDL_mixer -lSDL_gfx -g

all:$(OBJS)
	g++ $(LIBS) $(OBJS) -o roflcoptah

%o:%cpp
	g++ $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) roflcoptah
