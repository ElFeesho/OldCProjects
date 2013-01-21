CC=gcc
EXES=

OBJS=main.o window.o groups.o

CFLAGS=`pkg-config --cflags gtk+-2.0 sqlite3` -Wall -pipe 
LIBS=`pkg-config --libs gtk+-2.0 sqlite3`

PROG=pmdbtool$(EXES)

all:$(OBJS)
	@$(CC) $(OBJS) $(LIBS) -o $(PROG)
	@echo "LD $(OBJS) -> $(PROG)"
%o:%c
	@$(CC) -c $(CFLAGS) $<
	@echo "CC $< -> $@"

win:
	@echo "Making Windows binary"
	@make -f Makefile.win

clean:
	@rm -f $(PROG) $(OBJS)
	@echo Cleaning $(PROG) and $(OBJS)
