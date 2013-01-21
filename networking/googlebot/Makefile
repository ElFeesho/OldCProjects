OBJS=main.o con.o parse.o interp.o http.o
CFLAGS=
LIBS=
PROG=gbot
all:$(OBJS)
	gcc -o $(PROG) $(OBJS) $(LIBS)

%o:%c
	gcc -c $(CFLAGS) $<

clean:
	rm -f $(PROG) $(OBJS)
