OBJS=main.o sonix_compress.o bayer.o
CFLAGS=`pkg-config --cflags gtk+-2.0` -c
LIBS=`pkg-config --libs gtk+-2.0`
PROG=lwebcam2

all:$(OBJS)
	gcc -m32 $(OBJS) $(LIBS) -o $(PROG)

%o:%c
	gcc -m32 $(CFLAGS) $<

clean:
	rm -f $(OBJS) $(PROG)
