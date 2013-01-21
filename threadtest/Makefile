OBJS=main.o
#CFLAGS=-O3 -pipe -ffast-math
LIBS=-lpthread -lm

all:$(OBJS)
	gcc $(OBJS) -o threadtest $(LIBS)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm $(OBJS) threadtest

