#ifndef SHARED_H
#define SHARED_H

#include <SDL.h>

#define RAM_SIZE 0x1000
#define PROG_MEM_START 0x200
#define STACK_SIZE 16
#define HEX_SPRITE_LEN 5
#define HEX_SPRITE_START_ADDR 0
#define UPDATE_INTERVAL_SECONDS 1/240.f
#define TIMER_UPDATE_INTERVAL_SECONDS 1/60.f

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
};

enum vm_error {
    NO_ERROR,
    ERROR_STACK_OVERFLOW,
    ERROR_STACK_UNDERFLOW,
    ERROR_OUT_OF_BOUNDS_MEMORY_ACCESS
};

struct chip8 {
    uint8_t ram[RAM_SIZE];
    uint16_t stack[STACK_SIZE];
    uint16_t pc;
    uint16_t sp;
    uint16_t reg_i;
    uint8_t reg_dt;
    uint8_t reg_st;
    uint8_t reg_v[16];
    enum vm_error error;
    uint16_t prog_mem_end;
    float sec_since_update;
    float sec_since_dt_update;
    float sec_since_st_update;
    struct screen *screen;

    bool awaiting_input;
    uint8_t input_register;
};

#endif
