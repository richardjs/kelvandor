CFLAGS=-std=c99 -Wall -O3


objects=state.o stateio.o layout.o mcts.o random.o


all: kelvandor


kelvandor: $(objects) main.o


test: $(objects)


httpapi: kelvandor
	FLASK_APP=httpapi.py FLASK_ENV=development flask run


wasm:
	emcc -Wall -c state.c -o state.o
	emcc -Wall -c stateio.c -o stateio.o
	emcc -Wall -c layout.c -o layout.o
	emcc -Wall --bind wasmlib.cpp state.o stateio.o layout.o \
				--pre-js pre.js -o kelvandor.js
	mkdir -p ../wasm
	mv kelvandor.wasm ../wasm
	mv kelvandor.js ../wasm
	rm -f state.o stateio.o layout.o


wasmtest:
	make clean
	emmake make test
	node test
	make clean


clean:
	rm -f *.o
	rm -f kelvandor
	rm -f test
	rm -f test.wasm
	rm -fr __pycache__


# vim: set noexpandtab:
