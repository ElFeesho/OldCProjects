OBJS=main.o
LIBS=`pkg-config --libs gtk+-2.0 gthread-2.0 sdl` -lm -lwiiuse -lSDL_mixer
CFLAGS=`pkg-config --cflags sdl gtk+-2.0`

PROG=feeshsticks

all:$(OBJS)
	gcc $(OBJS) -o $(PROG) $(LIBS)

%.o:%.c
	gcc $(CFLAGS) -c $<

clean:
	rm $(OBJS) $(PROG) -f
