OBJS=main.o filelist.o format.o
CFLAGS=-O2 -pipe
LIBS=-lm

all:$(OBJS)
	@gcc $(OBJS) $(LIBS) -o nlls
	@echo LD nlls

%o:%c
	@gcc -c $(CFLAGS) $<
	@echo CC $@

clean:
	@rm -f $(OBJS) nlls
	@echo CLEAN
