OBJS=firc.o firc_error.o
TOBJS=test.o
PROG=libfirc.a

all:$(OBJS)
	ar -q $(PROG) $(OBJS)

test:$(TOBJS)
	g++ test.o -lfirc -L./ -o test

%o:%cpp
	g++ -c -g $<

clean:
	rm -f $(PROG) $(OBJS)
