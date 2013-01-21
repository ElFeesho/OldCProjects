OBJS= gtkmain.o
TOBJS= main.o
LIBS=`pkg-config --libs gtk+-2.0` -g
CFLAGS=`pkg-config --cflags gtk+-2.0` -g
PROG=lwebcam

all:$(OBJS)
	gcc -o $(PROG) $(LIBS) $(OBJS)
	
term:$(TOBJS)
	gcc -o tlwebcam $(TOBJS)
	
%o:%c
	gcc -c $(CFLAGS) $<
	
clean:
	rm -f $(OBJS) $(PROG)
