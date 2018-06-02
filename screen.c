#include <string.h>
#include "screen.h"
#include "machine.h"

struct screen *init_screen() {
    struct screen *screen = malloc(sizeof(struct screen));
    clear_screen(screen);
    return screen;
}

void clear_screen(struct screen *screen) {
    memset(screen->screen, 0, SCREEN_WIDTH_PX * SCREEN_HEIGHT_PX);
    screen->changed = true;
}

int get_coordinate(int x, int y) {
    int wrapped_x = x % SCREEN_WIDTH_PX;
    int wrapped_y = y % SCREEN_HEIGHT_PX;
    return wrapped_y * SCREEN_WIDTH_PX + wrapped_x;
}

bool xor_pixel(struct screen *screen, int x, int y, uint8_t value) {
    if (value) {
        int coordinate = get_coordinate(x, y);
        uint8_t old_value = screen->screen[coordinate];
        screen->screen[coordinate] = old_value ^ value;

        if (!screen->changed) {
            screen->changed = true;
        }

        return old_value;
    } else {
        return false;
    }
}

bool get_pixel(struct screen *screen, int x, int y) {
    int coordinate = get_coordinate(x, y);
    return screen->screen[coordinate];
}
