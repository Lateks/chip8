#ifndef MACHINE_H
#define MACHINE_H

#define RAM_SIZE 0x1000
#define PROG_MEM_START 0x200

struct chip8 {
    uint8_t ram[RAM_SIZE];
    uint16_t stack[16];
    uint16_t pc;
    uint16_t sp;
    uint16_t reg_i;
    uint8_t reg_v[16];
    uint8_t reg_dt;
    uint8_t reg_st;
};

size_t vm_init_with_rom(struct chip8 *vm, const char *const filename);

void vm_run(struct chip8 *vm);

#endif
