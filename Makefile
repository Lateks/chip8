CUTEST_DIR=lib/CuTest
SDL2_INCLUDE_DIR=/usr/local/Cellar/sdl2/2.0.8/include/SDL2/

INCLUDES = -I$(CUTEST_DIR) -I${SDL2_INCLUDE_DIR}

OBJECTS = machine.o instructions.o sdl_system.o

CC=gcc
CFLAGS=-std=gnu18 -Wall -Wextra $(INCLUDES) -L/usr/local/lib -lSDL2

chip8: $(OBJECTS)

test: $(OBJECTS) lib/CuTest/CuTest.o
