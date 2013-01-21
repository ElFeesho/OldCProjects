OBJS=main.o database.o errors.o board.o splash.o browser.o draw.o misc.o question.o whiteb.o groups.o levels.o
LIBS=`pkg-config --libs gtk+-2.0 sqlite3 librsvg-2.0`
CFLAGS=`pkg-config --cflags gtk+-2.0 sqlite3 librsvg-2.0` -Wall
PROG=powermaths

all:$(OBJS)
	gcc -o $(PROG) $(OBJS) $(LIBS)

%o:%c
	@gcc -c $(CFLAGS) $< 
	@echo CC $<

clean:
	rm -f $(PROG) $(OBJS)
