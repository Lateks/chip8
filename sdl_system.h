#ifndef SDL_SYSTEM_H
#define SDL_SYSTEM_H

#include <stdbool.h>
#include <SDL.h>

#define SCALE_MULTIPLIER 10

typedef struct io_state {
    SDL_Window *window;
    bool quit;
} io_state;

struct io_state init_io(int screen_width, int screen_height);

void handle_events(struct io_state *state);

bool is_key_down(uint8_t hex_key_code);

void close(struct io_state *state);

#endif
