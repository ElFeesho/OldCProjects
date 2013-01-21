OBJS=main.o window.o objects.o panels.o menu.o canvas.o
CFLAGS=`pkg-config --cflags gtk+-2.0`
LIBS=`pkg-config --libs gtk+-2.0`


all:$(OBJS)
	gcc $(OBJS) -o nlcomic $(LIBS)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -f nlcomic $(OBJS)
