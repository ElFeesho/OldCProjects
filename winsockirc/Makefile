OBJS=main.o
CFLAGS=
LIBS=-lwsock32
PROG=wsockirc.exe
CC=i586-mingw32msvc-g++

all:$(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $(PROG) 

%o:%cpp
	$(CC) $(CFLAGS) -c $<

clean:
	rm $(OBJS) $(PROG) -f
