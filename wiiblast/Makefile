OBJS=main.o bplate.o
CFLAGS=`sdl-config --cflags`
LIBS=-lwiiuse -lSDL -lSDL_mixer

all:$(OBJS)
	gcc -o wiiblast $(OBJS) $(LIBS)

%o:%c
	gcc $(CFLAGS) -c $< 

clean:
	rm $(OBJS)
