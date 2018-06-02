#ifndef MACHINE_H
#define MACHINE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define RAM_SIZE 0x1000
#define PROG_MEM_START 0x200
#define STACK_SIZE 16
#define SCREEN_WIDTH_PX 64
#define SCREEN_HEIGHT_PX 32
#define HEX_SPRITE_LEN 5
#define HEX_SPRITE_START_ADDR 0

enum vm_error {
    NO_ERROR,
    ERROR_STACK_OVERFLOW,
    ERROR_STACK_UNDERFLOW,
    ERROR_OUT_OF_BOUNDS_MEMORY_ACCESS,
    ERROR_INTEGER_OVERFLOW
};

struct chip8 {
    uint8_t ram[RAM_SIZE];
    uint16_t stack[STACK_SIZE];
    uint16_t pc;
    uint16_t sp;
    uint16_t reg_i;
    uint8_t reg_dt;
    uint8_t reg_st;
    uint8_t reg_v[16];
    enum vm_error error;
    uint16_t prog_mem_end;
    uint8_t screen[SCREEN_WIDTH_PX * SCREEN_HEIGHT_PX];
    bool draw_flag;
};

size_t vm_init_with_rom(struct chip8 *vm, const char *const filename);

void vm_run(struct chip8 *vm);

void clear_screen(struct chip8 *vm);

bool xor_pixel(struct chip8 *vm, int x, int y, bool value);

#endif
