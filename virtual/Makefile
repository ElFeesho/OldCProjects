OBJS=main.o
CFLAGS=-g -march=core2 -pipe
CXXFLAGS=$(CFLAGS)

all:$(OBJS)
	g++ $(OBJS) -o virtual

%o:%cpp
	g++ $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS)
