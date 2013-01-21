COBJS=client.o
SOBJS=server.o

all:
	@echo "Please do make client, or make server"

server:$(SOBJS)
	gcc $(SOJBS) -o fudpsrv

client:$(COBJS)
	gcc $(COBJS) -o fudpcl

clean:
	rm *.o fudpcl fudpsrv -f

%o:%c
	gcc -c $<
