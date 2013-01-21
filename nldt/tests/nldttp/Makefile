C_OBJS=client.o
S_OBJS=server.o
LIBS=-L./ -lnldttp `pkg-config --libs glib` -lm
CFLAGS=`pkg-config --cflags glib`

all:
	@echo Building NLDTTP Lib
	@echo Removing old lib.
	@rm libnldttp.a -f
	gcc nldttp.c -c $(CFLAGS)
	ar -q libnldttp.a nldttp.o
	@echo Done
	
client:$(C_OBJS)
	gcc $(C_OBJS) $(LIBS) -o client

server:$(S_OBJS)
	gcc $(S_OBJS) $(LIBS) -o server
	
%o:%c
	gcc $(CFLAGS) -c $<
