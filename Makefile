ARM = /home/chris/lunch/mbed/gcc4mbed/gcc-arm-none-eabi/bin/arm-none-eabi-
ARMCC = $(ARM)gcc

all: amd64

dual: dual.c dual.h Makefile
	clang dual.c -lSDL2 -o dual

dual.s: dual.c dual.h Makefile
	$(ARMCC) -std=c99 -mcpu=cortex-m0plus -mthumb -Os -S dual.c -o dual.s

amd64: amd64.c amd64.h dual.c dual.h Makefile
	clang -g amd64.c dual.c -lSDL2 -o amd64

test: test.c dual.c
	clang -g test.c dual.c -o test
	./test

