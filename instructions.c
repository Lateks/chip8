#include "instructions.h"

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

void run_ld_i_addr(struct chip8 *vm, uint16_t instruction) {
    vm->reg_i = MEM_ADDR(instruction);
}

void run_ld_vx_vy(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg1 = REG_1(instruction);
    uint8_t reg2 = REG_2(instruction);

    vm->reg_v[reg1] = vm->reg_v[reg2];
}
