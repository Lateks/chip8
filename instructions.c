#include <stdbool.h>
#include <stdlib.h>
#include "instructions.h"
#include "sdl_system.h"

#define OP_VX_VY(op, vm, instruction) \
    uint8_t reg1 = REG_1(instruction); \
    uint8_t reg2 = REG_2(instruction); \
    vm->reg_v[reg1] = vm->reg_v[reg1] op vm->reg_v[reg2];

void run_ret(struct chip8 *vm) {
    if (vm->sp > 0) {
        vm->pc = vm->stack[vm->sp - 1];
        --vm->sp;
    } else {
        vm->error = ERROR_STACK_UNDERFLOW;
    }
}

void run_jp_addr(struct chip8 *vm, uint16_t instruction) {
    vm->pc = MEM_ADDR(instruction);
}

void run_call_addr(struct chip8 *vm, uint16_t instruction) {
    if (vm->sp < STACK_SIZE) {
        vm->stack[vm->sp++] = vm->pc;
        vm->pc = MEM_ADDR(instruction);
    } else {
        vm->error = ERROR_STACK_OVERFLOW;
    }
}

void run_se_vx_byte(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    uint8_t byte = LOW_BYTE(instruction);

    if (vm->reg_v[reg] == byte) {
        vm->pc += 4;
    }
}

void run_se_vx_vy(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg1 = REG_1(instruction);
    uint8_t reg2 = REG_2(instruction);

    if (vm->reg_v[reg1] == vm->reg_v[reg2]) {
        vm->pc += 4;
    }
}

void run_sne_vx_byte(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    uint8_t byte = LOW_BYTE(instruction);

    if (vm->reg_v[reg] != byte) {
        vm->pc += 4;
    }
}

void run_ld_vx_byte(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    uint8_t operand = LOW_BYTE(instruction);

    vm->reg_v[reg] = operand;
}

void run_add_vx_byte(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    uint8_t byte = LOW_BYTE(instruction);

    if (vm->reg_v[reg] <= UINT8_MAX - byte) {
        vm->reg_v[reg] += byte;
    } else {
        vm->error = ERROR_INTEGER_OVERFLOW;
    }
}

void run_sne_vx_vy(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg1 = REG_1(instruction);
    uint8_t reg2 = REG_2(instruction);

    if (vm->reg_v[reg1] != vm->reg_v[reg2]) {
        vm->pc += 4;
    }
}

void run_ld_i_addr(struct chip8 *vm, uint16_t instruction) {
    vm->reg_i = MEM_ADDR(instruction);
}

void run_ld_vx_vy(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg1 = REG_1(instruction);
    uint8_t reg2 = REG_2(instruction);

    vm->reg_v[reg1] = vm->reg_v[reg2];
}

void run_or_vx_vy(struct chip8 *vm, uint16_t instruction) {
    OP_VX_VY(|, vm, instruction);
}

void run_and_vx_vy(struct chip8 *vm, uint16_t instruction) {
    OP_VX_VY(&, vm, instruction);
}

void run_xor_vx_vy(struct chip8 *vm, uint16_t instruction) {
    OP_VX_VY(^, vm, instruction);
}

void run_add_vx_vy(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg1 = REG_1(instruction);
    uint8_t reg2 = REG_2(instruction);
    uint16_t result = vm->reg_v[reg1] + vm->reg_v[reg2];

    vm->reg_v[0xF] = vm->reg_v[reg1] > UINT8_MAX - vm->reg_v[reg2];
    vm->reg_v[reg1] = LOW_BYTE(result);
}

void run_sub_vx_vy(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg1 = REG_1(instruction);
    uint8_t reg2 = REG_2(instruction);

    uint8_t result = (int8_t) vm->reg_v[reg1] - (int8_t) vm->reg_v[reg2];
    vm->reg_v[0xF] = vm->reg_v[reg1] > vm->reg_v[reg2];
    vm->reg_v[reg1] = result;
}

void run_shr_vx(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg1 = REG_1(instruction);

    vm->reg_v[0xF] = LSB(vm->reg_v[reg1]);
    vm->reg_v[reg1] = vm->reg_v[reg1] >> 1;
}

void run_subn_vx_vy(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg1 = REG_1(instruction);
    uint8_t reg2 = REG_2(instruction);

    uint8_t result = (int8_t) vm->reg_v[reg2] - (int8_t) vm->reg_v[reg1];
    vm->reg_v[0xF] = vm->reg_v[reg2] > vm->reg_v[reg1];
    vm->reg_v[reg1] = result;
}

void run_shl_vx(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg1 = REG_1(instruction);

    vm->reg_v[0xF] = MSB(vm->reg_v[reg1]);
    vm->reg_v[reg1] = vm->reg_v[reg1] << 1;
}

void run_jp_v0_addr(struct chip8 *vm, uint16_t instruction) {
    vm->pc = vm->reg_v[0] + MEM_ADDR(instruction);
}

void run_rnd_vx_byte(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    uint8_t r = rand() % 255u;
    uint8_t byte = LOW_BYTE(instruction);

    vm->reg_v[reg] = r & byte;
}

void run_drw_vx_vy_n(struct chip8 *vm, uint16_t instruction) {
    int n = LOW_NIBBLE(instruction);
    int x_coord = vm->reg_v[REG_1(instruction)];
    int y_coord = vm->reg_v[REG_2(instruction)];
    uint16_t start = vm->reg_i;
    int i, j;
    uint8_t byte;

    vm->reg_v[0xF] = 0;
    for (i = 0; i < n; ++i) {
        byte = vm->ram[start + i];
        for (j = 7; j >= 0; --j) {
            if (xor_pixel(vm, x_coord + j, y_coord + i, LSB(byte))) {
                vm->reg_v[0xF] = 1;
            }
            byte >>= 1;
        }
    }
    vm->draw_flag = true;
}

void run_skp_vx(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    if (is_key_down(vm->reg_v[reg])) {
        vm->pc += 4;
    }
}

void run_sknp_vx(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    if (!is_key_down(vm->reg_v[reg])) {
        vm->pc += 4;
    }
}

void run_ld_f_vx(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    vm->reg_i = HEX_SPRITE_START_ADDR + vm->reg_v[reg] * HEX_SPRITE_LEN;
}

void run_ld_b_vx(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    uint8_t regValue = vm->reg_v[reg];

    uint8_t hundredsDigit = regValue / 100;
    uint8_t tensDigit = (regValue - hundredsDigit * 100) / 10;
    uint8_t lowDigit = regValue % 10;

    uint16_t location = vm->reg_i;
    vm->ram[location] = hundredsDigit;
    vm->ram[location + 1] = tensDigit;
    vm->ram[location + 2] = lowDigit;
}

void run_ld_vx_i(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    uint16_t location = vm->reg_i;
    int i;
    for (i = 0; i <= reg; ++i) {
        vm->reg_v[i] = vm->ram[location++];
    }
}
