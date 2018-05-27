#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define UPDATE_INTERVAL_SECONDS 1/60.f

#define RAM_SIZE 0x1000
#define PROG_MEM_START 0x200

size_t read_rom(uint8_t *ram, const char *const filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Cannot open file %s.\n", filename);
        exit(1);
    }

    int max_size = RAM_SIZE - PROG_MEM_START;
    size_t bytes_read = fread(ram, sizeof(uint8_t), max_size, fp);
    fclose(fp);

    return bytes_read;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        puts("Usage: chip8 path/to/rom");
        exit(0);
    }

    uint8_t ram[RAM_SIZE];
    uint16_t stack[16];
    uint16_t pc = PROG_MEM_START, sp = 0, reg_i;
    uint8_t reg_v[16], reg_dt, reg_st;

    float sec_since_update = 0;
    int loop_start = clock();
    int temp = 0;

    size_t bytes_read = read_rom(&ram[PROG_MEM_START], argv[1]);
    printf("Successfully read %lu bytes from file\n", bytes_read);

    while (true) {
        if (sec_since_update >= UPDATE_INTERVAL_SECONDS) {
            puts("Updating...");
            // TODO: read instruction
            // TODO: execute instruction
            // TODO: update timers
            sec_since_update = 0;
        }
        temp = clock();
        sec_since_update += (temp - loop_start) / (CLOCKS_PER_SEC * 1.f);
        loop_start = temp;
    }
}
