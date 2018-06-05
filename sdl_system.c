#include <math.h>
#include "sdl_system.h"
#include "screen.h"

#define WAV_FILE "sound/316852__kwahmah-02__1khz-30-seconds.wav"

SDL_Scancode hex_key_scancode_map[] = {
    SDL_SCANCODE_N, // 0x0,
    SDL_SCANCODE_5, // 0x1
    SDL_SCANCODE_6, // 0x2
    SDL_SCANCODE_7, // 0x3
    SDL_SCANCODE_T, // 0x4
    SDL_SCANCODE_Y, // 0x5
    SDL_SCANCODE_U, // 0x6
    SDL_SCANCODE_G, // 0x7,
    SDL_SCANCODE_H, // 0x8,
    SDL_SCANCODE_J, // 0x9,
    SDL_SCANCODE_B, // 0xA,
    SDL_SCANCODE_M, // 0xB,
    SDL_SCANCODE_8, // 0xC,
    SDL_SCANCODE_I, // 0xD,
    SDL_SCANCODE_K, // 0xE,
    SDL_SCANCODE_COMMA // 0xF
};

SDL_Keycode hex_key_keycode_map[] = {
    SDLK_n, // 0x0
    SDLK_5, // 0x1
    SDLK_6, // 0x2
    SDLK_7, // 0x3
    SDLK_t, // 0x4
    SDLK_y, // 0x5
    SDLK_u, // 0x6
    SDLK_g, // 0x7
    SDLK_h, // 0x8
    SDLK_j, // 0x9
    SDLK_b, // 0xA
    SDLK_m, // 0xB
    SDLK_8, // 0xC
    SDLK_i, // 0xD
    SDLK_k, // 0xE
    SDLK_COMMA // 0xF
};

void audio_callback(void *userdata, Uint8 *stream, int len) {
    struct audio_data *data = (struct audio_data *)userdata;

    Uint32 len_remaining = data->wav_len_bytes - data->audio_pos;
    Uint32 copy_bytes = fmin(len, data->wav_len_bytes);
    Uint32 start_pos = copy_bytes > len_remaining ? 0 : data->audio_pos;
    Uint8 *buffer_start = data->wav_buffer + start_pos;

    SDL_memset(stream, 0, copy_bytes);
    memcpy(stream, buffer_start, copy_bytes);

    data->audio_pos = start_pos + copy_bytes;
}

void init_window(struct io_state *state, int screen_width, int screen_height) {
    state->screen_width = screen_width;
    state->screen_height = screen_height;
    state->window = SDL_CreateWindow(
            "CHIP-8",
            SDL_WINDOWPOS_UNDEFINED,
            SDL_WINDOWPOS_UNDEFINED,
            screen_width * SCALE_MULTIPLIER,
            screen_height * SCALE_MULTIPLIER,
            SDL_WINDOW_SHOWN);
    if (state->window == NULL) {
        printf("Failed to create SDL Window. Error: %s\n", SDL_GetError());
        state->quit = true;
    } else {
        state->renderer = SDL_CreateRenderer(state->window, -1, SDL_RENDERER_ACCELERATED);
        if (state->renderer == NULL) {
            printf("Failed to create SDL Renderer. Error: %s\n", SDL_GetError());
            state->quit = true;
        }
    }
}

void init_sound(struct io_state *state) {
    Uint32 wav_length;
    Uint8 *wav_buffer;
    SDL_AudioSpec audio_spec;

    if (SDL_LoadWAV(WAV_FILE, &audio_spec, &wav_buffer, &wav_length) == NULL) {
        printf("Failed to load WAW file. Error: %s\n", SDL_GetError());
        state->audio_data.loaded = false;
        state->quit = true;
    } else {
        state->playing_sound = false;

        SDL_zero(audio_spec);
        audio_spec.callback = audio_callback;
        audio_spec.userdata = &state->audio_data;
        audio_spec.samples = 32;

        state->audio_data.loaded = true;
        state->audio_data.wav_buffer = wav_buffer;
        state->audio_data.audio_pos = 0;
        state->audio_data.wav_len_bytes = wav_length;

        if (SDL_OpenAudio(&audio_spec, NULL) < 0){
            printf("Couldn't open sound device: %s\n", SDL_GetError());
            state->quit = true;
        }
    }
}

void init_io(struct io_state *state, int screen_width, int screen_height) {
    state->window = NULL;
    state->quit = false;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Failed to initialize SDL. Error: %s\n", SDL_GetError());
        state->quit = true;
    } else {
        init_window(state, screen_width, screen_height);
        init_sound(state);
    }
}

void handle_events(struct io_state *state, int *key_pressed) {
    SDL_Event e;
    *key_pressed = -1;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            state->quit = true;
        } else if (e.type == SDL_KEYDOWN) {
            int hex_key;
            for (hex_key = 0; hex_key < 16; ++hex_key) {
                if (hex_key_keycode_map[hex_key]) {
                    break;
                }
            }
            if (hex_key < 16) {
                *key_pressed = hex_key;
            }
        }
    }
}

void close(struct io_state *state) {
    if (state->window) {
        SDL_DestroyWindow(state->window);
        state->window = NULL;
    }
    if (state->renderer) {
        SDL_DestroyRenderer(state->renderer);
        state->renderer = NULL;
    }
    if (state->audio_data.loaded) {
        SDL_CloseAudio();
        SDL_FreeWAV(state->audio_data.wav_buffer);
    }

    SDL_Quit();
}

bool is_key_down(uint8_t hex_key_code) {
    if (hex_key_code > 0xF) {
        return false;
    }
    const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
    const SDL_Scancode scancode = hex_key_scancode_map[hex_key_code];
    return keyboard_state[scancode];
}

void draw_screen(struct io_state *state, const struct screen * const screen) {
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 0);
    SDL_RenderClear(state->renderer);

    for (int y = 0; y < state->screen_height; ++y) {
        for (int x = 0; x < state->screen_width; ++x) {
            if (get_pixel(screen, x, y)) {
                SDL_Rect fillRect = {
                    x * SCALE_MULTIPLIER,
                    y * SCALE_MULTIPLIER,
                    SCALE_MULTIPLIER,
                    SCALE_MULTIPLIER
                };
                SDL_SetRenderDrawColor(state->renderer, 0xFF, 0xFF, 0xFF, 0xFF);
                SDL_RenderFillRect(state->renderer, &fillRect);
            }
        }
    }

    SDL_RenderPresent(state->renderer);
}

void play_sound(struct io_state *state) {
    if (!state->playing_sound && state->audio_data.loaded) {
        state->playing_sound = true;
        SDL_PauseAudio(0);
    }
}

void stop_sound(struct io_state *state) {
    if (state->playing_sound && state->audio_data.loaded) {
        state->playing_sound = false;
        SDL_PauseAudio(1);
        state->audio_data.audio_pos = 0;
    }
}
