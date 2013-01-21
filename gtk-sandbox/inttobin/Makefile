OBJS=main.o
CFLAGS=`pkg-config --cflags gtk+-2.0` -g
LIBS=`pkg-config --libs gtk+-2.0`

all:$(OBJS)
	gcc $(OBJS) -o inttobin $(LIBS)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) inttobin
