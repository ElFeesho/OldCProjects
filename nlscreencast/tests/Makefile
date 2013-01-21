TEST_OBJS=test.o bitmap.o
LIBS=-lX11 -lpng12
CFLAGS=
test:$(TEST_OBJS)
	gcc $(TEST_OBJS) -o test_screen $(LIBS)

all:
	@echo WIP!

%o:%c
	gcc -c $(CFLAGS) $<

clean:
	rm -rf $(TEST_OBJS) test_screen
