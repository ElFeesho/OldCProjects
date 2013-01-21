OBJS=main.o window.o toolbar.o toolbox.o menu.o draw.o about.o gtkvimage.o \
toolopts.o
CFLAGS=`pkg-config --cflags gtk+-2.0 librsvg-2.0` -Wall
LIBS=`pkg-config --libs gtk+-2.0 librsvg-2.0`


all:$(OBJS)
	gcc $(OBJS) $(LIBS) -o pmed

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -rf $(OBJS) pmed
