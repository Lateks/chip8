#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define UPDATE_INTERVAL_SECONDS 1/60.f

#define PROG_MEM_START 0x200

int main() {
    uint8_t ram[0x1000];
    uint16_t stack[0x10];
    uint16_t pc, sp, reg_i;
    uint8_t reg_v[0x10], reg_dt, reg_st;

    float sec_since_update = 0;
    int loop_start = clock();
    int temp = 0;
    while (true) {
        if (sec_since_update >= UPDATE_INTERVAL_SECONDS) {
            puts("Updating...");
            // TODO: read instruction
            // TODO: execute instruction
            // TODO: update timers
            sec_since_update = 0;
        }
        temp = clock();
        sec_since_update += (temp - loop_start) / (CLOCKS_PER_SEC * 1.f);
        loop_start = temp;
    }
}
