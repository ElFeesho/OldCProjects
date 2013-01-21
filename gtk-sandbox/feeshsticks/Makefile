OBJS=main.o
LIBS=`pkg-config --libs openal freealut vorbis vorbisfile ogg gtk+-2.0` -lm -lcwiid
CFLAGS=`pkg-config --cflags openal freealut vorbis vorbisfile ogg gtk+-2.0`

PROG=feeshsticks

all:$(OBJS)
	gcc $(OBJS) -o $(PROG) $(LIBS)

%.o:%.c
	gcc $(CFLAGS) -c $<

clean:
	rm $(OBJS) $(PROG) -f
