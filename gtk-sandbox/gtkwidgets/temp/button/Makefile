OBJS=main.o pmbutton.o
CFLAGS=`pkg-config --cflags gtk+-2.0 librsvg-2.0` -Wall
LIBS=`pkg-config --libs gtk+-2.0 librsvg-2.0`

all:$(OBJS)
	gcc $(LIBS) $(OBJS) -o test -g

%o:%c
	gcc $(CFLAGS) -c -g $<

clean:
	rm $(OBJS) test
