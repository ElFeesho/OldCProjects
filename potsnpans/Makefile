OBJS=main.o res.o
CFLAGS=`sdl-config --cflags` -g
LIBS=`sdl-config --libs` -lwiiuse -lSDL_image -lSDL_mixer -g

all:$(OBJS)
	gcc $(OBJS) -o potsnpans $(LIBS)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) potsnpans
