OBJS=main.o client.o socket.o
LIBS=
CFLAGS=
CXXFLAGS=$(CFLAGS)

all:$(OBJS)
	g++ $(OBJS) -o feeshServ $(LIBS)

%o:%cpp
	g++ $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS)
