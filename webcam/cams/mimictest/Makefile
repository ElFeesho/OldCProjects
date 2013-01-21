OBJS=main.o image_manip.o bitstring.o colorspace.o deblock.o decode.o encode.o fdct_quant.o idct_dequant.o mimic.o vlc_common.o vlc_decode.o vlc_encode.o
LIBS=`pkg-config --libs gtk+-2.0` -L./ -lgnet-2.0 -lm
CFLAGS=`pkg-config --cflags gtk+-2.0 gnet-2.0` -lm
PROG=mimictest

all:$(OBJS)
	gcc -m32 $(OBJS) -o $(PROG) $(LIBS)

%o:%c
	gcc -m32 $(CFLAGS) -c $<

clean:	
	rm -f *.o
