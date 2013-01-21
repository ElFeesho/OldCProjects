#############################
#       Nonlogic Pad        #
#                           #
#   Nonlogic Team Project   #
#############################

OBJS=main.o network.o globals.o con_diag.o error.o nick_diag.o grid_diag.o toolbar.o colour_widget.o draw.o main_window.o config.o chat.o client.o string_ops.o chan_diag.o colour_diag.o about_diag.o banner.o image_dump.o resources.o

CFLAGS=`pkg-config --cflags gtk+-2.0 gnet-2.0 && xml2-config --cflags` -Wall -g
LIBS=`pkg-config --libs gtk+-2.0 gnet-2.0 && xml2-config --libs` -g

PROG=nlpad

all:$(OBJS)
	gcc -o $(PROG) $(OBJS) $(LIBS)

%o:%c
	gcc -c $(CFLAGS) $<

win:
	make -f Makefile.win

clean:
	rm -f $(PROG) $(OBJS)

