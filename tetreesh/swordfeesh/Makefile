OBJS=engine.o entity.o entitylist.o image.o scene.o sprite.o inputevent.o transition.o
LIBS=
CFLAGS=`sdl-config --cflags` -DSDLWINDOW -g
CXXFLAGS=$(CFLAGS)

LIB=libswordfeesh.a


all:$(LIB)
	@echo Making library

$(LIB):$(OBJS)
	ar -q $(LIB) $(OBJS)

%o:%cpp
	g++ $(CXXFLAGS) -c $<

install:
	cp *.hpp ../include
	cp $(LIB) ../lib

clean:
	rm -f $(OBJS) $(LIB)
