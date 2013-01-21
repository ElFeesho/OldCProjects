OBJS=main.o scan.o
PROG=hexscan
LIBS=
CFLAGS=-O3 -pipe

all:$(OBJS)
	gcc -o $(PROG) $(OBJS) $(LIBS)	

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -f *.o $(PROG)
