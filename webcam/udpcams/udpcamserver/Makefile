OBJS=main.o bitstring.o colorspace.o deblock.o decode.o encode.o fdct_quant.o idct_dequant.o mimic.o vlc_common.o vlc_decode.o vlc_encode.o
LIBS=`pkg-config --libs gtk+-2.0 gnet-2.0` -lm
CFLAGS=`pkg-config --cflags gtk+-2.0 gnet-2.0` -I./
PROG=udpcamserver

all:$(OBJS)
	gcc $(OBJS) -o $(PROG) $(LIBS)

%o:%c
	gcc $(CFLAGS) -c $<

clean:
	rm *.o -f
