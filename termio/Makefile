OBJS=main.o
CFLAGS=-O2 -pipe
LIBS=-lm -lasound
PROG=termtest

all:$(OBJS)
	gcc -o $(PROG) $(OBJS) $(LIBS)
	
%o:%c
	gcc -c $(CFLAGS) $<

clean:
	rm -f $(OBJS) $(PROG)
