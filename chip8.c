#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>
#include "machine.h"

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

    size_t bytes_read = vm_init_with_rom(&vm, argv[1]);
    printf("Successfully read %lu bytes from file\n", bytes_read);

    srand(time(NULL));

    while (true) {
        if (sec_since_update >= UPDATE_INTERVAL_SECONDS) {
            vm_run(&vm);
            sec_since_update = 0;
        }
        temp = clock();
        sec_since_update += (temp - loop_start) / (CLOCKS_PER_SEC * 1.f);
        loop_start = temp;
    }
}
