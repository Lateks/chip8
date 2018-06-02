#ifndef SCREEN_H
#define SCREEN_H

#include <stdbool.h>
#include <stdint.h>

#define SCREEN_WIDTH_PX 64
#define SCREEN_HEIGHT_PX 32

struct screen {
    uint8_t screen[SCREEN_WIDTH_PX * SCREEN_HEIGHT_PX];
    bool changed;
};

struct screen *init_screen();

void clear_screen(struct screen *screen);

bool xor_pixel(struct screen *screen, int x, int y, uint8_t value);

bool get_pixel(struct screen *screen, int x, int y);

#endif
