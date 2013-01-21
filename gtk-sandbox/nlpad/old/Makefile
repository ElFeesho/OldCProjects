CFLAGS=`pkg-config gtk+-2.0 gnet-2.0 --cflags && xml2-config --cflags` -Wall
LIBS=`pkg-config gtk+-2.0 gnet-2.0 --libs && xml2-config --libs`
OBJS=main.o chanDialog.o config.o globals.o callbacks.o helper.o gui.o

all: $(OBJS)
	gcc -g $(CFLAGS) $(LIBS) -o nlpad $(OBJS)

%.o : %.c
	gcc -g $(CFLAGS) -c $< -o $@

clean:
	-rm -rf *.o
	-rm nlpad

