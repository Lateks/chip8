#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include <SDL.h>
#include "machine.h"
#include "sdl_system.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        puts("Usage: chip8 path/to/rom");
        exit(0);
    }

    struct chip8 vm;

    int loop_start = clock();
    int temp = 0;
    float dt = 1;

    vm_init_with_rom(&vm, argv[1]);
    struct io_state state = init_io(SCREEN_WIDTH_PX, SCREEN_HEIGHT_PX);
    srand(time(NULL));

    while (!state.quit) {
        handle_events(&state);

        vm_run(&vm, dt);
        vm_update_timers(&vm, dt);
        if (vm.draw_flag) {
            draw_screen(&state, &vm);
            vm.draw_flag = false;
        }

        temp = clock();
        dt = (temp - loop_start) / (CLOCKS_PER_SEC * 1.f);
        loop_start = temp;
    }

    close(&state);
}
