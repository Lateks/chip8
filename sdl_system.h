#ifndef SDL_SYSTEM_H
#define SDL_SYSTEM_H

#include <stdbool.h>
#include "shared.h"

#define SCALE_MULTIPLIER 10

void init_io(struct io_state *state, int screen_width, int screen_height);

void handle_events(struct io_state *state, int *key_pressed);

bool is_key_down(uint8_t hex_key_code);

void close(struct io_state *state);

void draw_screen(struct io_state *state, const struct chip8 * const vm);

void play_sound(struct io_state *state);

void stop_sound(struct io_state *state);

#endif
