OBJS=main.o
CFLAGS=-g
LIBS=
PROG=bintodec

all:$(OBJS)
	gcc $(OBJS) $(LIBS) -o $(PROG)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -rf $(OBJS) $(PROG)
