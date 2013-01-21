OBJS=main_desktop.o main.o font.o locator.o
CFLAGS=`pkg-config sdl --cflags`
LIBS=`pkg-config --libs sdl` -lSDL_image -lGL -lGLU

PROG=beach-test

all:$(OBJS)
	g++ -o $(PROG) $(OBJS) $(LIBS)

%o:%cpp
	g++ $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(PROG)
