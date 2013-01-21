OBJS=ball.o ent.o main.o paddle.o engine.o SDLW.o score.o lpaddle.o
CFLAGS=`sdl-config --cflags` -pipe -O2 -g -I/usr/include/lua5.2
LIBS=`sdl-config --libs` -lSDL_image -L/usr/local/lib/ -llua -g
PROG=pong

all:$(OBJS)
	g++ $(OBJS) $(LIBS) -g -o $(PROG)

%.o:%.cpp
	g++ $(CFLAGS) -c $<

clean:
	rm $(OBJS) $(PROG) -f
