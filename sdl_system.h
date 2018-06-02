#ifndef SDL_SYSTEM_H
#define SDL_SYSTEM_H

#include <stdbool.h>
#include <SDL.h>
#include "machine.h"

#define SCALE_MULTIPLIER 10

struct io_state {
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool quit;
};

struct io_state init_io(int screen_width, int screen_height);

void handle_events(struct io_state *state, int *key_pressed);

bool is_key_down(uint8_t hex_key_code);

void close(struct io_state *state);

void draw_screen(struct io_state *state, const struct chip8 * const vm);

#endif
