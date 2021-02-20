CFLAGS=-std=c99 -Wall -O3

objects=state.o state_cli.o lookups.o

all: kelvandor

kelvandor: $(objects) main.o

test: $(objects)
	./test

clean:
	rm -f *.o
	rm -f kelvandor
	rm -f test


# vim: set noexpandtab:
