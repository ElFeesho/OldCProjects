OBJS=objs/main.o objs/database.o objs/errors.o objs/board.o \
objs/splash.o objs/browser.o objs/draw.o objs/misc.o objs/question.o \
objs/whiteb.o objs/groups.o objs/levels.o objs/types.o objs/topics.o \
objs/gtkpmbutton.o objs/stopic.o objs/gtkpmhbutton.o objs/search.o \
objs/gtkvimage.o objs/scat.o objs/colour_box.o objs/tool_box.o
LIBS=`pkg-config --libs gtk+-2.0 sqlite3 librsvg-2.0`
CFLAGS=`pkg-config --cflags gtk+-2.0 sqlite3 librsvg-2.0` -Wall -g -O0 -pipe
PROG=powermaths

all:$(OBJS)
	@gcc -o $(PROG) $(OBJS) $(LIBS)
	@echo LDD $(OBJS)

objs/%o:source/%c
	@gcc -c $(CFLAGS) -o $@ $< 
	@echo CC $<
	
win:
	@echo "Making Windows binary"
	@make -f Makefile.win
clean:
	rm -f $(PROG) $(OBJS)
