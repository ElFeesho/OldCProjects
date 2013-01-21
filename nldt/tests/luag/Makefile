OBJS=main.o
CFLAGS=`pkg-config --cflags gtk+-2.0` -I/usr/local/include
LIBS=`pkg-config --libs gtk+-2.0` -L/usr/local/lib/ -llua
PROG=luag

all:$(OBJS)
	gcc $(OBJS) $(LIBS) -o $(PROG)

%.o:%.c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(PROG)
