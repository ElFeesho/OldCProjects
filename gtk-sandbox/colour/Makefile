OBJS=main.o colour.o
CFLAGS=`pkg-config --cflags gtk+-2.0`
LIBS=`pkg-config --libs gtk+-2.0`
PROG=colour_test

all:$(OBJS)
	gcc -o $(PROG) $(OBJS) $(LIBS)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -fv $(OBJS) $(PROG)
