#ifndef MACHINE_H
#define MACHINE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "shared.h"

size_t vm_init_with_rom(struct chip8 *vm, const char *const filename);

void vm_run(struct chip8 *vm, float dt, struct io_state *io);

bool should_redraw(struct chip8 *vm);

void reset_redraw_flag(struct chip8 *vm);

void cleanup(struct chip8 *vm);

void vm_receive_input(struct chip8 *vm, int hex_key);

bool should_play_sound(struct chip8 *vm);

#endif
