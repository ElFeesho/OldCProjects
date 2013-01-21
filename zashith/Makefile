OBJS=main.o
CFLAGS=
LIBS=
PROG=zashith

all:$(OBJS)
	gcc -o $(PROG) $(OBJS) $(LIBS)

%o:%c
	gcc $(CFLAGS) $< -c

clean:
	rm -f $(OBJS) $(PROG)
