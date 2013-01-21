OBJS=main.o
PROG=audaciousxchat.so
CFLAGS=-fPIC `pkg-config --cflags dbus-glib-1 audacious audclient`
LIBS=-shared -fPIC `pkg-config --libs dbus-glib-1 audacious audclient`

all:$(OBJS)
	gcc $(OBJS) -o $(PROG) $(LIBS)

%o:%c
	gcc $(CFLAGS) $< -c

clean:
	rm -rf $(OBJS) $(PROG)

