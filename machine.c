#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "shared.h"
#include "machine.h"
#include "instructions.h"
#include "screen.h"
#include "sdl_system.h"

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
    vm->awaiting_input = false;
    vm->prog_mem_end = PROG_MEM_START + program_size;
    vm->screen = init_screen();

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

void print_error(struct chip8* vm, uint16_t old_pc) {
    switch (vm->error) {
        case NO_ERROR:
            break;
        case ERROR_STACK_OVERFLOW:
            printf("Error: Stack overflow at %04x\n", old_pc);
            break;
        case ERROR_STACK_UNDERFLOW:
            printf("Error: Stack underflow at %04x\n", old_pc);
            break;
        case ERROR_OUT_OF_BOUNDS_MEMORY_ACCESS:
            printf("Error: Out of bounds memory access at %04x\n", old_pc);
            break;
        default:
            printf("Error: Unknown error\n");
    }
}

void vm_run_instruction(struct chip8 *vm, float dt) {
    vm->sec_since_update += dt;
    if (vm->sec_since_update < UPDATE_INTERVAL_SECONDS || vm->awaiting_input) {
        return;
    }

    uint16_t instruction = read_instruction(vm);
    uint16_t instruction_type = HIGH_NIBBLE(instruction);
    uint16_t old_pc = vm->pc;

    vm->pc += 2;

    switch (instruction_type) {
        case 0:
            switch (instruction) {
                case CLS:
                    run_cls(vm);
                    break;
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
        case 0xD:
            run_drw_vx_vy_n(vm, instruction);
            break;
        case 0xE:
            switch (LOW_BYTE(instruction)) {
                case 0x9E:
                    run_skp_vx(vm, instruction);
                    break;
                case 0xA1:
                    run_sknp_vx(vm, instruction);
                    break;
                default:
                    printf("Skipping unknown 0xEnnn instruction %x\n", instruction);
                    break;
            }
            break;
        case 0xF:
            switch (LOW_BYTE(instruction)) {
                case 0x7:
                    run_ld_vx_dt(vm, instruction);
                    break;
                case 0xA:
                    run_ld_vx_k(vm, instruction);
                    break;
                case 0x15:
                    run_ld_dt_vx(vm, instruction);
                    break;
                case 0x18:
                    run_ld_st_vx(vm, instruction);
                    break;
                case 0x1E:
                    run_add_i_vx(vm, instruction);
                    break;
                case 0x29:
                    run_ld_f_vx(vm, instruction);
                    break;
                case 0x33:
                    run_ld_b_vx(vm, instruction);
                    break;
                case 0x55:
                    run_ld_i_vx(vm, instruction);
                    break;
                case 0x65:
                    run_ld_vx_i(vm, instruction);
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
        print_error(vm, old_pc);
        exit(vm->error);
    }

    vm->sec_since_update = 0;
}

void vm_update_timers(struct chip8 *vm, float dt) {
    vm->sec_since_dt_update += dt;
    vm->sec_since_st_update += dt;
    if (vm->awaiting_input) return;

    if (vm->sec_since_dt_update > TIMER_UPDATE_INTERVAL_SECONDS) {
        if (vm->reg_dt > 0) --vm->reg_dt;
        vm->sec_since_dt_update = 0;
    }
    if (vm->sec_since_st_update > TIMER_UPDATE_INTERVAL_SECONDS) {
        if (vm->reg_st > 0) --vm->reg_st;
        vm->sec_since_st_update = 0;
    }
}

void vm_run(struct chip8 *vm, float dt, struct io_state *io) {
    uint8_t old_st = vm->reg_st;

    vm_run_instruction(vm, dt);
    vm_update_timers(vm, dt);

    if (vm->reg_st != old_st) {
        if (vm->reg_st > 0) {
            play_sound(io);
        } else {
            stop_sound(io);
        }
    }
}

bool should_redraw(struct chip8 *vm) {
    return vm->screen->changed;
}

void reset_redraw_flag(struct chip8 *vm) {
    vm->screen->changed = false;
}

void cleanup(struct chip8 *vm) {
    free(vm->screen);
    vm->screen = NULL;
}

void vm_receive_input(struct chip8 *vm, int hex_key) {
    if ((hex_key >= 0 || hex_key < 16) && vm->awaiting_input) {
        run_ld_vx_k_receive_input(vm, hex_key);
    }
}

bool should_play_sound(struct chip8 *vm) {
    return vm->reg_st > 0;
}
