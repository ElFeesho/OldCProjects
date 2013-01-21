CC = gcc
CFLAGS = -Wall -pipe `sdl-config --cflags` -g
LDFLAGS = -ldl `sdl-config --libs` -lSDL_mixer -lSDL_ttf -lSDL_image -g
BIN = openstage

C_SRC_FILES = $(wildcard *.c)

C_OBJS = $(C_SRC_FILES:%.c=%.o)

all: $(C_OBJS)
	$(CC) $(CFLAGS) $(C_OBJS) $(LDFLAGS) -o $(BIN)

clean:
	rm *.o &> /dev/null ; \

%.o: %.c
	$(CC) $(CFLAGS) -c $<

