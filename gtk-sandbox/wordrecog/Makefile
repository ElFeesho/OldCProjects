OBJS=main.o
CFLAGS=`pkg-config --cflags gtk+-2.0`
LIBS=`pkg-config --libs gtk+-2.0`

all:$(OBJS)
	gcc $(OBJS) $(LIBS) -o wordrecog

%o:%c
	gcc -c $(CFLAGS) $<

clean:
	rm -f $(OBJS) wordrecog
