OBJS=test.o
CFLAGS=-I/usr/include/python2.5 `pkg-config --cflags gtk+-2.0`
LIBS=-lpython2.5 `pkg-config --libs gtk+-2.0`
PROG=pythong

all:$(OBJS)
	gcc $(OBJS) $(LIBS) -o $(PROG)

%.o:%.c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(PROG)
