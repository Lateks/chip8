#include <stdlib.h>
#include <stdio.h>
#include "machine.h"

/*
 * Reads two-byte instruction stored in big-endian format
 * from RAM (starting at PC).
 */
uint16_t read_instruction(struct chip8 *vm) {
    uint8_t high_byte = vm->ram[vm->pc];
    uint8_t low_byte = vm->ram[vm->pc + 1];
    return high_byte << 8 | low_byte;
}

size_t vm_init_with_rom(struct chip8 *vm, const char *const filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Cannot open file %s.\n", filename);
        exit(1);
    }

    int max_size = RAM_SIZE - PROG_MEM_START;
    size_t bytes_read = fread(&vm->ram[PROG_MEM_START], sizeof(uint8_t), max_size, fp);
    fclose(fp);

    vm->pc = PROG_MEM_START;
    vm->sp = 0;

    return bytes_read;
}

void vm_run(struct chip8 *vm) {
    uint16_t instruction = read_instruction(vm);

    printf("Read unknown instruction %x\n", instruction);

    vm->pc = vm->pc + 2;
}
