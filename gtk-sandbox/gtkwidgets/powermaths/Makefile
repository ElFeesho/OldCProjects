OBJS=main.o gtkpmbutton.o
CFLAGS=`pkg-config --cflags gtk+-2.0 librsvg-2.0`
LIBS=`pkg-config --libs gtk+-2.0 librsvg-2.0`
PROG=button_test

all:$(OBJS)
	gcc $(OBJS) $(LIBS) -o $(PROG)

%o:%c
	gcc -c $(CFLAGS) $<

clean:
	rm $(OBJS) $(PROG) -f
