OBJS=main.o image_manip.o
LIBS=`pkg-config --libs gtk+-2.0 gnet-2.0 libmimic`
CFLAGS=`pkg-config --cflags gtk+-2.0 gnet-2.0 libmimic`
PROG=mimictest

all:$(OBJS)
	gcc $(OBJS) -o $(PROG) $(LIBS)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(PROG) $(OBJS) 
