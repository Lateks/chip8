CUTEST_DIR=lib/CuTest

CC=gcc
CFLAGS=-std=gnu18 -Wall -Wextra -I$(CUTEST_DIR)

chip8: machine.o instructions.o

test: instructions.o machine.o lib/CuTest/CuTest.o
