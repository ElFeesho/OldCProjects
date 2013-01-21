OBJS=main.o
CFLAGS=
LIBS=-lncurses

all:$(OBJS)
	gcc $(OBJS) $(LIBS) -o ntest	

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -rf $(OBJS) ntest
