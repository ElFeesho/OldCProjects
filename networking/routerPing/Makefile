OBJS=main.o net.o
CFLAGS=
LIBS=

all:$(OBJS)
	gcc -o routerPing $(OBJS) $(LIBS)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OBJS)
