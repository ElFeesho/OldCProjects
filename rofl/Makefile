DEPS="sdl SDL_image SDL_mixer GL GLU"
OBJS=main.o sync.o
CFLAGS=`pkg-config --cflags $(DEPS)` -I./include
CXXFLAGS=$(CFLAGS)
LIBS=-L./lib  -lswordfeesh `pkg-config --libs $(DEPS)` -lGL -lGLU

all:$(OBJS)
	g++ $(OBJS) $(LIBS) -o game1

%o:%cpp
	g++ $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) game1
