OBJS=main.o
CFLAGS=
LIBS=-lX11
PROG=test

all:$(OBJS)
	gcc $(OBJS) -o $(PROG) $(LIBS)

%.o:%.c
	gcc $(CFLAGS) -c $<

clean:
	rm test -f
	rm *.o -f
