OBJS=main.o
CFLAGS=`pkg-config --cflags ogg vorbis vorbisfile`
LIBS=`pkg-config --libs ogg vorbis vorbisfile`
PROG=oggtest

all:$(OBJS)
	gcc $(OBJS) $(LIBS) -o $(PROG)

%.o:%.c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(PROG)
