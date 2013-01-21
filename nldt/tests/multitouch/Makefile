OBJS = main.o Camera.o QuickCam.o
LIBS = `sdl-config --libs`
CFLAGS = `sdl-config --cflags`
BIN = multitouch
CXX = g++

all: $(OBJS)
	$(CXX) $(LIBS) $(OBJS) -o $(BIN)

%.o: %.cpp
	$(CXX) $(CFLAGS) $< -c -o $@

clean:
	rm -f $(BIN) $(OBJS)

