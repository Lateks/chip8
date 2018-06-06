CUTEST_DIR=lib/CuTest
SDL2_INCLUDE_DIR=/usr/local/Cellar/sdl2/2.0.8/include/SDL2/

INCLUDES = -I$(CUTEST_DIR) -I${SDL2_INCLUDE_DIR}

OBJECTS = machine.o instructions.o sdl_system.o screen.o

WARNING_FLAGS = -Wall -Wextra -Werror=format-security -Werror=implicit-function-declaration

CFLAGS=$(WARNING_FLAGS) $(INCLUDES)
LDFLAGS=-L/usr/local/lib -lSDL2

chip8: $(OBJECTS)

test: $(OBJECTS) lib/CuTest/CuTest.o

clean:
	rm -f ${OBJECTS} chip8

clean-test:
	rm -f ${OBJECTS} test
