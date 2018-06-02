#include "sdl_system.h"
#include "machine.h"
#include "screen.h"

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
        } else {
            state.renderer = SDL_CreateRenderer(state.window, -1, SDL_RENDERER_ACCELERATED);
            if (state.renderer == NULL) {
                printf("Failed to create SDL Renderer. Error: %s\n", SDL_GetError());
                state.quit = true;
            }
        }
    }

    return state;
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

void draw_screen(struct io_state *state, const struct chip8 * const vm) {
    SDL_SetRenderDrawColor(state->renderer, 0, 0, 0, 0);
    SDL_RenderClear(state->renderer);

    for (int y = 0; y < SCREEN_HEIGHT_PX; ++y) {
        for (int x = 0; x < SCREEN_WIDTH_PX; ++x) {
            if (get_pixel(vm->screen, x, y)) {
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
