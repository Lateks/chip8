#include <stdlib.h>
#include <stdio.h>
#include "machine.h"
#include "instructions.h"

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
    vm->error = 0;
    vm->prog_mem_end = PROG_MEM_START + bytes_read;

    return bytes_read;
}

void run_8xyn(struct chip8 *vm, uint16_t instruction) {
    uint8_t optype = LOW_NIBBLE(instruction);

    switch (optype) {
        case 0:
            run_ld_vx_vy(vm, instruction);
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
                    printf("Skipping unknown 0nnn instruction\n");
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
                printf("Skipping unknown 5nnn instruction\n");
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
                printf("Skipping unknown 9nnn instruction\n");
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
