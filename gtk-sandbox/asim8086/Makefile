OBJS=main.o asm.o gui.o
LIBS=`pkg-config --libs gtk+-2.0`
CFLAGS=`pkg-config --cflags gtk+-2.0` -g
PROG=asim8086

all:$(OBJS)
	gcc -m32 $(OBJS) -o $(PROG) $(LIBS)

%o:%c
	gcc -m32 $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(PROG)
