#include "sdl_system.h"

SDL_Scancode hex_key_map[] = {
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

struct io_state init_io(int screen_width, int screen_height) {
    struct io_state state = { .window = NULL, .quit = false };

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL. Error: %s\n", SDL_GetError());
        state.quit = true;
    } else {
        state.window = SDL_CreateWindow(
                "CHIP-8",
                SDL_WINDOWPOS_UNDEFINED,
                SDL_WINDOWPOS_UNDEFINED,
                screen_width * SCALE_MULTIPLIER,
                screen_height * SCALE_MULTIPLIER,
                SDL_WINDOW_SHOWN);
        if (state.window == NULL) {
            printf("Failed to create SDL Window. Error: %s\n", SDL_GetError());
            state.quit = true;
        }
    }

    return state;
}

void handle_events(struct io_state *state) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
        if (e.type == SDL_QUIT) {
            state->quit = true;
        }
    }
}

void close(struct io_state *state) {
    if (state->window) {
        SDL_DestroyWindow(state->window);
        SDL_Quit();
    }
}

bool is_key_down(uint8_t hex_key_code) {
    if (hex_key_code > 0xF) {
        return false;
    }
    const Uint8 *keyboard_state = SDL_GetKeyboardState(NULL);
    const SDL_Scancode scancode = hex_key_map[hex_key_code];
    return keyboard_state[scancode];
}
