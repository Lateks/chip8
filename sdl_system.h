#ifndef SDL_SYSTEM_H
#define SDL_SYSTEM_H

#include <SDL.h>
#include <stdbool.h>

#define SCALE_MULTIPLIER 10

struct screen;

struct audio_data {
    bool loaded;
    Uint8 *wav_buffer;
    Uint32 audio_pos;
    Uint32 wav_len_bytes;
};

struct io_state {
    SDL_Window *window;
    SDL_Renderer *renderer;
    bool quit;
    bool playing_sound;
    struct audio_data audio_data;
    int screen_width;
    int screen_height;
};

void init_io(struct io_state *state, int screen_width, int screen_height);

void handle_events(struct io_state *state, int *key_pressed);

bool is_key_down(uint8_t hex_key_code);

void close(struct io_state *state);

void draw_screen(struct io_state *state, const struct screen * const screen);

void play_sound(struct io_state *state);

void stop_sound(struct io_state *state);

#endif
