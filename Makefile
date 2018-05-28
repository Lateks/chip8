CUTEST_DIR=lib/CuTest

CFLAGS=-Wall -Wextra -pedantic -I$(CUTEST_DIR)

chip8: machine.o

test: instructions.o machine.o lib/CuTest/CuTest.o
