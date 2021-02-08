CFLAGS=-std=c99 -Wall -O3

objects=state.o state_cli.o

all: kelvandor

kelvandor: $(objects) main.o

test: $(objects)

clean:
	rm *.o
	rm kelvandor
	rm test


# vim: set noexpandtab:
