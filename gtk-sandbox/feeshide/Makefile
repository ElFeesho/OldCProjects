OBJS=callback.o filelist.o main.o
CFLAGS=`pkg-config --cflags gtksourceview-1.0 gtk+-2.0`
LIBS=`pkg-config --libs gtksourceview-1.0 gtk+-2.0`

all:$(OBJS)
	gcc $(OBJS) -o feeshide $(LIBS)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) feeshide
