CFLAGS=-std=c99 -Wall -O3


objects=state.o stateio.o layout.o


all: kelvandor


kelvandor: $(objects) main.o


test: $(objects)


wasm:
	emcc -c state.c -o state.o
	emcc -c stateio.c -o stateio.o
	emcc -c layout.c -o layout.o
	emcc --bind wasmlib.cpp state.o stateio.o layout.o -o kelvandor.js


wasm_dist: wasm
	mkdir -p ../wasm
	mv *.wasm ../wasm
	mv *.js ../wasm


clean:
	rm -f *.o
	rm -f *.js
	rm -f *.wasm
	rm -f kelvandor
	rm -f test


# vim: set noexpandtab:
