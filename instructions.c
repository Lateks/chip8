#include <stdbool.h>
#include <stdlib.h>
#include "instructions.h"
#include "sdl_system.h"
#include "screen.h"

#define OP_VX_VY(op, vm, instruction) \
    uint8_t reg1 = REG_1(instruction); \
    uint8_t reg2 = REG_2(instruction); \
    vm->reg_v[reg1] = vm->reg_v[reg1] op vm->reg_v[reg2];

void run_cls(struct chip8 *vm) {
    clear_screen(&vm->screen);
}

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
        vm->pc += 2;
    }
}

void run_se_vx_vy(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg1 = REG_1(instruction);
    uint8_t reg2 = REG_2(instruction);

    if (vm->reg_v[reg1] == vm->reg_v[reg2]) {
        vm->pc += 2;
    }
}

void run_sne_vx_byte(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    uint8_t byte = LOW_BYTE(instruction);

    if (vm->reg_v[reg] != byte) {
        vm->pc += 2;
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

    vm->reg_v[reg] += byte;
}

void run_sne_vx_vy(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg1 = REG_1(instruction);
    uint8_t reg2 = REG_2(instruction);

    if (vm->reg_v[reg1] != vm->reg_v[reg2]) {
        vm->pc += 2;
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
    int sprite_bytes = LOW_NIBBLE(instruction);
    int x_coord = vm->reg_v[REG_1(instruction)];
    int y_coord = vm->reg_v[REG_2(instruction)];
    uint16_t start = vm->reg_i;
    int i, j;
    uint8_t byte;
    uint8_t pixel;

    vm->reg_v[0xF] = 0;
    for (i = 0; i < sprite_bytes; ++i) {
        byte = vm->ram[start + i];
        for (j = 7; j >= 0; --j) {
            pixel = LSB(byte);
            if (xor_pixel(&vm->screen, x_coord + j, y_coord + i, pixel)) {
                vm->reg_v[0xF] = 1;
            }
            byte >>= 1;
        }
    }
}

void run_skp_vx(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    if (is_key_down(vm->reg_v[reg])) {
        vm->pc += 2;
    }
}

void run_sknp_vx(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    if (!is_key_down(vm->reg_v[reg])) {
        vm->pc += 2;
    }
}

void run_ld_vx_dt(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    vm->reg_v[reg] = vm->reg_dt;
}

void run_ld_vx_k(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    vm->awaiting_input = true;
    vm->input_register = reg;
}

void run_ld_vx_k_receive_input(struct chip8 *vm, uint8_t input) {
    vm->reg_v[vm->input_register] = input;
    vm->awaiting_input = false;
}

void run_ld_dt_vx(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    vm->reg_dt = vm->reg_v[reg];
    vm->sec_since_dt_update = 0;
}

void run_ld_st_vx(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    vm->reg_st = vm->reg_v[reg];
    vm->sec_since_st_update = 0;
}

void run_add_i_vx(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    vm->reg_i += vm->reg_v[reg];
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

void run_ld_i_vx(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    uint16_t location = vm->reg_i;
    uint8_t i;
    for (i = 0; i <= reg; ++i) {
        vm->ram[location++] = vm->reg_v[i];
    }
}

void run_ld_vx_i(struct chip8 *vm, uint16_t instruction) {
    uint8_t reg = REG_1(instruction);
    uint16_t location = vm->reg_i;
    uint8_t i;
    for (i = 0; i <= reg; ++i) {
        vm->reg_v[i] = vm->ram[location++];
    }
}

void run_8xyn(struct chip8 *vm, uint16_t instruction) {
    uint8_t optype = LOW_NIBBLE(instruction);

    switch (optype) {
        case 0:
            run_ld_vx_vy(vm, instruction);
            break;
        case 1:
            run_or_vx_vy(vm, instruction);
            break;
        case 2:
            run_and_vx_vy(vm, instruction);
            break;
        case 3:
            run_xor_vx_vy(vm, instruction);
            break;
        case 4:
            run_add_vx_vy(vm, instruction);
            break;
        case 5:
            run_sub_vx_vy(vm, instruction);
            break;
        case 6:
            run_shr_vx(vm, instruction);
            break;
        case 7:
            run_subn_vx_vy(vm, instruction);
            break;
        case 0xE:
            run_shl_vx(vm, instruction);
            break;
        default:
            printf("Skipping unknown 8xyn instruction %x\n", instruction);
    }
}

void run_instruction(struct chip8 *vm, uint16_t instruction) {
    uint16_t instruction_type = HIGH_NIBBLE(instruction);

    switch (instruction_type) {
        case 0:
            switch (instruction) {
                case CLS:
                    run_cls(vm);
                    break;
                case RET:
                    run_ret(vm);
                    break;
                default:
                    printf("Skipping unknown 0nnn instruction %x\n", instruction);
            }
            break;
        case 1:
            run_jp_addr(vm, instruction);
            break;
        case 2:
            run_call_addr(vm, instruction);
            break;
        case 3:
            run_se_vx_byte(vm, instruction);
            break;
        case 4:
            run_sne_vx_byte(vm, instruction);
            break;
        case 5:
            if (LOW_NIBBLE(instruction) == 0) {
                run_se_vx_vy(vm, instruction);
            } else {
                printf("Skipping unknown 5nnn instruction %x\n", instruction);
            }
            break;
        case 6:
            run_ld_vx_byte(vm, instruction);
            break;
        case 7:
            run_add_vx_byte(vm, instruction);
            break;
        case 8:
            run_8xyn(vm, instruction);
            break;
        case 9:
            if (LOW_NIBBLE(instruction) == 0) {
                run_sne_vx_vy(vm, instruction);
            } else {
                printf("Skipping unknown 9nnn instruction %x\n", instruction);
            }
            break;
        case 0xA:
            run_ld_i_addr(vm, instruction);
            break;
        case 0xB:
            run_jp_v0_addr(vm, instruction);
            break;
        case 0xC:
            run_rnd_vx_byte(vm, instruction);
            break;
        case 0xD:
            run_drw_vx_vy_n(vm, instruction);
            break;
        case 0xE:
            switch (LOW_BYTE(instruction)) {
                case 0x9E:
                    run_skp_vx(vm, instruction);
                    break;
                case 0xA1:
                    run_sknp_vx(vm, instruction);
                    break;
                default:
                    printf("Skipping unknown 0xEnnn instruction %x\n", instruction);
                    break;
            }
            break;
        case 0xF:
            switch (LOW_BYTE(instruction)) {
                case 0x7:
                    run_ld_vx_dt(vm, instruction);
                    break;
                case 0xA:
                    run_ld_vx_k(vm, instruction);
                    break;
                case 0x15:
                    run_ld_dt_vx(vm, instruction);
                    break;
                case 0x18:
                    run_ld_st_vx(vm, instruction);
                    break;
                case 0x1E:
                    run_add_i_vx(vm, instruction);
                    break;
                case 0x29:
                    run_ld_f_vx(vm, instruction);
                    break;
                case 0x33:
                    run_ld_b_vx(vm, instruction);
                    break;
                case 0x55:
                    run_ld_i_vx(vm, instruction);
                    break;
                case 0x65:
                    run_ld_vx_i(vm, instruction);
                    break;
                default:
                    printf("Skipping unknown 0xFnnn instruction %x\n", instruction);
                    break;
            }
            break;
        default:
            printf("Skipping unknown instruction %x\n", instruction);
    }
}

