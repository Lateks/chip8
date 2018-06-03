#ifndef MACHINE_H
#define MACHINE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include "shared.h"

size_t vm_init_with_rom(struct chip8 *vm, const char *const filename);

void vm_run(struct chip8 *vm, float dt, struct io_state *io);

void vm_receive_input(struct chip8 *vm, int hex_key);

#endif
