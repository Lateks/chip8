SDL_INCLUDES = `pkg-config --cflags-only-I sdl2`
CUTEST_DIR = lib/CuTest
INCLUDES = $(SDL_INCLUDES) -I$(CUTEST_DIR)

OBJECTS = machine.o instructions.o sdl_system.o screen.o

WARNING_FLAGS = -Wall -Wextra -Werror=format-security -Werror=implicit-function-declaration

CFLAGS = $(WARNING_FLAGS) $(INCLUDES) `pkg-config --cflags-only-other sdl2`
LDFLAGS = `pkg-config --libs sdl2` -lm

chip8: $(OBJECTS)

test: $(OBJECTS) lib/CuTest/CuTest.o

clean:
	rm -f ${OBJECTS} chip8

clean-test:
	rm -f ${OBJECTS} test
