OBJS=main.o desktop.o
CFLAGS=`pkg-config --cflags gtk+-2.0 gnet-2.0`
LIBS=`pkg-config --libs gtk+-2.0 gnet-2.0`
PROG=nlproj

all:$(OBJS)
	@gcc $(OBJS) $(LIBS) -o $(PROG)
	@echo "LDD $(PROG)"
	
%o:%c
	@gcc -c $(CFLAGS) $<
	@echo "CC $<"

tar:
	@cp *.c nlproj/
	@cp *.h nlproj/
	@cp *.rc nlproj/
	@cp Makefile nlproj/
	@tar czf nlproj.tar.gz nlproj/
	@echo "Tarred!"
	
clean:
	@rm -f $(PROG) $(OBJS)
	@echo "Clean!"
