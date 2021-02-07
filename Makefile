CFLAGS=-std=c99 -Wall -O3

all: kelvandor

kelvandor: state.o state_cli.o main.o

clean:
	rm *.o
	rm kelvandor
