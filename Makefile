all: amd64

dual: dual.c dual.h Makefile
	clang dual.c -lSDL2 -o dual

dual.s: dual.c dual.h Makefile
	clang -Os -S dual.c -o dual.s

amd64: amd64.c amd64.h dual.c dual.h Makefile
	clang amd64.c dual.c -lSDL2 -o amd64

