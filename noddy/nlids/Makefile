OBJS=main.o sha1.o
CFLAGS=-g -Wall -Werror -pedantic -ansi
LIBS=

all:$(OBJS)
	gcc $(OBJS) -o nlids $(LIBS) -static

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) 
