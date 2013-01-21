OBJS=main.o ffile.o
CFLAGS=-pipe -O2 -g

all:$(OBJS)
	@gcc $(OBJS) -g -o test_ffile
	@echo LD $(OBJS) -o test_ffile

%o:%c
	@gcc -c $(CFLAGS) $<
	@echo "CC -c $(CFLAGS) $<"

clean:
	rm -rf *.o
