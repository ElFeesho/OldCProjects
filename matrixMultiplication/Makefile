OBJS=main.o
LIBS=
CFLAGS=
PROG=mmulti

all:$(OBJS)
	gcc $(OBJS) -o $(PROG) $(LIBS)

%o:%c
	gcc -c $(CFLAGS) $<

clean:
	rm -f $(PROG) $(OBJS)
