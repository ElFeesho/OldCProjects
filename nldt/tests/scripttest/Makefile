OBJS=.main.o .gizmos.o .gtk_binds.o .draw_binds.o .sound.o .loader.o
CFLAGS=`pkg-config --cflags gtk+-2.0 openal freealut vorbis vorbisfile ogg gtksourceview-2.0` -I/usr/local/include -Wall -pipe
LIBS=`pkg-config --libs openal freealut gtk+-2.0 vorbis vorbisfile ogg gtksourceview-2.0` -L/usr/local/lib/ -llua
PROG=scripttest

all:$(OBJS)
	gcc $(OBJS) $(LIBS) -o $(PROG)

.%.o:%.c
	gcc $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(PROG)
