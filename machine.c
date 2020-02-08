#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "machine.h"
#include "instructions.h"
#include "screen.h"
#include "sdl_system.h"

#define UPDATE_INTERVAL_SECONDS (1/120.f)
#define TIMER_UPDATE_INTERVAL_SECONDS (1/120.f)
#define RENDER_INTERVAL_SECONDS (1/60.f)

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
    clear_screen(&vm->screen);

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
    uint16_t old_pc = vm->pc;

    vm->pc += 2;

    run_instruction(vm, instruction);

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

void vm_render(struct chip8 *vm, float dt, struct io_state *io) {
    vm->sec_since_render += dt;
    if (vm->screen.changed && vm->sec_since_render >= RENDER_INTERVAL_SECONDS) {
        draw_screen(io, &vm->screen);
        vm->screen.changed = false;
        vm->sec_since_render = 0;
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

    vm_render(vm, dt, io);
}

void vm_receive_input(struct chip8 *vm, int hex_key) {
    if ((hex_key >= 0 || hex_key < 16) && vm->awaiting_input) {
        run_ld_vx_k_receive_input(vm, hex_key);
    }
}
