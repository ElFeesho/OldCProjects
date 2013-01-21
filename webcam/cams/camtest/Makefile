OBJS=main.o camera.o manipulate.o gui.o controls.o bayer.o network.o bitstring.o colorspace.o deblock.o decode.o encode.o fdct_quant.o idct_dequant.o mimic.o vlc_common.o vlc_decode.o vlc_encode.o
CFLAGS=`pkg-config --cflags gtk+-2.0` -g
LIBS=`pkg-config --libs gtk+-2.0` -lm -g
PROG=camera-test

all:$(OBJS)
	gcc -m32 $(OBJS) $(LIBS) -o $(PROG)

%o:%c
	gcc -m32 $(CFLAGS) -c $<

clean:
	rm $(OBJS) $(PROG) -f

