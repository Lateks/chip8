#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <SDL.h>
#include "machine.h"
#include "sdl_system.h"
#include "screen.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        puts("Usage: chip8 path/to/rom");
        exit(0);
    }

    struct chip8 vm;
    struct io_state state;

    clock_t loop_start = clock();
    clock_t temp = 0;
    float dt = 1;
    int keypress = -1;

    vm_init_with_rom(&vm, argv[1]);
    init_io(&state, SCREEN_WIDTH_PX, SCREEN_HEIGHT_PX);
    srand(time(NULL));

    while (!state.quit) {
        handle_events(&state, &keypress);

        if (keypress > -1 && vm.awaiting_input) {
            vm_receive_input(&vm, keypress);
        }
        vm_run(&vm, dt, &state);

        temp = clock();
        dt = (temp - loop_start) / (CLOCKS_PER_SEC * 1.f);
        loop_start = temp;
    }

    close(&state);
}
