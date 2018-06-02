#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <SDL.h>
#include "machine.h"
#include "sdl_system.h"

#define UPDATE_INTERVAL_SECONDS 1/60.f

int main(int argc, char *argv[]) {
    if (argc < 2) {
        puts("Usage: chip8 path/to/rom");
        exit(0);
    }

    struct chip8 vm;

    float sec_since_update = 0;
    int loop_start = clock();
    int temp = 0;

    vm_init_with_rom(&vm, argv[1]);
    struct io_state state = init_io(SCREEN_WIDTH_PX, SCREEN_HEIGHT_PX);
    srand(time(NULL));

    while (!state.quit) {
        handle_events(&state);

        if (sec_since_update >= UPDATE_INTERVAL_SECONDS) {
            vm_run(&vm);
            sec_since_update = 0;
        }
        if (vm.draw_flag) {
            draw_screen(&state, &vm);
            vm.draw_flag = false;
        }

        temp = clock();
        sec_since_update += (temp - loop_start) / (CLOCKS_PER_SEC * 1.f);
        loop_start = temp;
    }

    close(&state);
}
