OBJS=main.o
LIBS=
CFLAGS=
CC=gcc
PROG=sjmp

all:$(OBJS)
	$(CC) $(LIBS) $(OBJS) -o $(PROG)

%o:%c
	$(CC) $(CFLAGS) -c $<

clean:
	rm $(OBJS) $(PROG) -f
