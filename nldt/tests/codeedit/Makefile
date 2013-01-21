OBJS=main.o codeedit.o 
LIBS=`pkg-config --libs gtksourceview-2.0 gtk+-2.0`
CFLAGS=`pkg-config --cflags gtk+-2.0 gtksourceview-2.0`
PROG=codeedit

all:$(OBJS)
	gcc -o $(PROG) $(OBJS) $(LIBS)

%.o:%.c
	gcc $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(PROG)
