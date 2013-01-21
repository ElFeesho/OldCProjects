OBJS=main.o
LIBS=`pkg-config --libs gtk+-2.0 librsvg-2.0` 
CFLAGS=`pkg-config --cflags gtk+-2.0 librsvg-2.0`

all:$(OBJS)
	gcc $(OBJS) -o svg_test $(LIBS)

%o:%c
	gcc $(CFLAGS) -c $<
