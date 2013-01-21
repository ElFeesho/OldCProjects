OBJS=main.o
LIBS=-lfmodex
CFLAGS=-pipe -I/usr/local/include
PROG=voip

all:$(OBJS)
	gcc $(OBJS) $(LIBS) -o voip
	
%o:%c
	gcc $(CFLAGS) -c $<
	
clean:
	rm $(OBJS) $(PROG) -f