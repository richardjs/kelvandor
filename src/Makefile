CFLAGS=-std=c99 -Wall -O3


objects=state.o stateio.o layout.o


all: kelvandor


kelvandor: $(objects) main.o


test: $(objects)


wasm:
	emcc -Wall -c state.c -o state.o
	emcc -Wall -c stateio.c -o stateio.o
	emcc -Wall -c layout.c -o layout.o
	emcc -Wall --bind wasmlib.cpp state.o stateio.o layout.o -o kelvandor.js
	mkdir -p ../wasm
	mv *.wasm ../wasm
	mv *.js ../wasm
	rm -f state.o stateio.o layout.o


clean:
	rm -f *.o
	rm -f *.js
	rm -f *.wasm
	rm -f kelvandor
	rm -f test


# vim: set noexpandtab:
