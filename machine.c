#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "machine.h"
#include "instructions.h"

uint8_t hex_sprites[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0x0
    0x20, 0x60, 0x20, 0x20, 0x70, // 0x1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 0x2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 0x3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 0x4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 0x5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 0x6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 0x7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 0x8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 0x9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // 0xA
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // 0xB
    0xF0, 0x80, 0x80, 0x80, 0xF0, // 0xC
    0xE0, 0x90, 0x90, 0x90, 0xE0, // 0xD
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // 0xE
    0xF0, 0x80, 0xF0, 0x80, 0x80  // 0xF
};

/*
 * Reads two-byte instruction stored in big-endian format
 * from RAM (starting at PC).
 */
uint16_t read_instruction(struct chip8 *vm) {
    uint8_t high_byte = vm->ram[vm->pc];
    uint8_t low_byte = vm->ram[vm->pc + 1];
    return high_byte << 8 | low_byte;
}

void init(struct chip8 *vm, size_t program_size) {
    vm->pc = PROG_MEM_START;
    vm->sp = 0;
    vm->error = 0;
    vm->prog_mem_end = PROG_MEM_START + program_size;

    memcpy(&vm->ram[HEX_SPRITE_START_ADDR], hex_sprites, 16 * HEX_SPRITE_LEN);
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

    init(vm, bytes_read);

    return bytes_read;
}

void run_8xyn(struct chip8 *vm, uint16_t instruction) {
    uint8_t optype = LOW_NIBBLE(instruction);

    switch (optype) {
        case 0:
            run_ld_vx_vy(vm, instruction);
            break;
        case 1:
            run_or_vx_vy(vm, instruction);
            break;
        case 2:
            run_and_vx_vy(vm, instruction);
            break;
        case 3:
            run_xor_vx_vy(vm, instruction);
            break;
        case 4:
            run_add_vx_vy(vm, instruction);
            break;
        case 5:
            run_sub_vx_vy(vm, instruction);
            break;
        case 6:
            run_shr_vx(vm, instruction);
            break;
        case 7:
            run_subn_vx_vy(vm, instruction);
            break;
        case 0xE:
            run_shl_vx(vm, instruction);
            break;
        default:
            printf("Skipping unknown 8xyn instruction %x\n", instruction);
    }
}

void print_error(struct chip8* vm) {
    switch (vm->error) {
        case NO_ERROR:
            break;
        case ERROR_STACK_OVERFLOW:
            printf("Error: Stack overflow at %04x\n", vm->pc);
            break;
        case ERROR_STACK_UNDERFLOW:
            printf("Error: Stack underflow at %04x\n", vm->pc);
            break;
        case ERROR_OUT_OF_BOUNDS_MEMORY_ACCESS:
            printf("Error: Out of bounds memory access at %04x\n", vm->pc);
            break;
        case ERROR_INTEGER_OVERFLOW:
            printf("Error: integer overflow at %04x\n", vm->pc);
            break;
        default:
            printf("Error: Unknown error\n");
    }
}

void vm_run(struct chip8 *vm) {
    uint16_t instruction = read_instruction(vm);
    uint16_t instruction_type = HIGH_NIBBLE(instruction);
    uint16_t old_pc = vm->pc;

    switch (instruction_type) {
        case 0:
            switch (instruction) {
                case RET:
                    run_ret(vm);
                    break;
                default:
                    printf("Skipping unknown 0nnn instruction %x\n", instruction);
            }
            break;
        case 1:
            run_jp_addr(vm, instruction);
            break;
        case 2:
            run_call_addr(vm, instruction);
            break;
        case 3:
            run_se_vx_byte(vm, instruction);
            break;
        case 4:
            run_sne_vx_byte(vm, instruction);
            break;
        case 5:
            if (LOW_NIBBLE(instruction) == 0) {
                run_se_vx_vy(vm, instruction);
            } else {
                printf("Skipping unknown 5nnn instruction %x\n", instruction);
            }
            break;
        case 6:
            run_ld_vx_byte(vm, instruction);
            break;
        case 7:
            run_add_vx_byte(vm, instruction);
            break;
        case 8:
            run_8xyn(vm, instruction);
            break;
        case 9:
            if (LOW_NIBBLE(instruction) == 0) {
                run_sne_vx_vy(vm, instruction);
            } else {
                printf("Skipping unknown 9nnn instruction %x\n", instruction);
            }
            break;
        case 0xA:
            run_ld_i_addr(vm, instruction);
            break;
        case 0xB:
            run_jp_v0_addr(vm, instruction);
            break;
        case 0xC:
            run_rnd_vx_byte(vm, instruction);
            break;
        case 0xF:
            switch (LOW_BYTE(instruction)) {
                case 0x29:
                    run_ld_f_vx(vm, instruction);
                    break;
                default:
                    printf("Skipping unknown 0xFnnn instruction %x\n", instruction);
                    break;
            }
            break;
        default:
            printf("Skipping unknown instruction %x\n", instruction);
    }


    if (vm->pc < PROG_MEM_START || vm->pc >= vm->prog_mem_end) {
        vm->error = ERROR_OUT_OF_BOUNDS_MEMORY_ACCESS;
    }

    if (vm->error) {
        print_error(vm);
        exit(vm->error);
    }

    if (vm->pc == old_pc) {
        vm->pc += 2;
    }

    if (vm->reg_dt > 0) --vm->reg_dt;
    if (vm->reg_st > 0) --vm->reg_st;
}
