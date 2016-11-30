all: amd64

dual: dual.c dual.h Makefile
	clang dual.c -lSDL2 -o dual

dual.lpc810.s: dual.c dual.h Makefile
	clang -O0 -S dual.c -o dual.lpc810.s

amd64: amd64.c amd64.h dual.c dual.h Makefile
	clang amd64.c dual.c -lSDL2 -o amd64

