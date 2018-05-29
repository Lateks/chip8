#ifndef MACHINE_H
#define MACHINE_H

#include <stdlib.h>
#include <stdint.h>

#define RAM_SIZE 0x1000
#define PROG_MEM_START 0x200
#define STACK_SIZE 16

#define ERROR_STACK_OVERFLOW 1

struct chip8 {
    uint8_t ram[RAM_SIZE];
    uint16_t stack[STACK_SIZE];
    uint16_t pc;
    uint16_t sp;
    uint16_t reg_i;
    uint8_t reg_v[16];
    uint8_t reg_dt;
    uint8_t reg_st;
    uint8_t error;
};

size_t vm_init_with_rom(struct chip8 *vm, const char *const filename);

void vm_run(struct chip8 *vm);

#endif
