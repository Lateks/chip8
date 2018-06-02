#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include <stdint.h>
#include "machine.h"

#define LOW_BYTE(instr) (instr & 0x00FF)
#define MEM_ADDR(instr) (instr & 0x0FFF)
#define HIGH_NIBBLE(instr) ((instr & 0xF000) >> 12)
#define REG_1(instr) ((instr & 0x0F00) >> 8)
#define REG_2(instr) ((instr & 0x00F0) >> 4)
#define LOW_NIBBLE(instr) (instr & 0x000F)
#define MSB(byte) (byte >> 7)
#define LSB(byte) (byte & 1)

#define CLS 0x00E0
#define RET 0x00EE

void run_ret(struct chip8 *vm);

void run_jp_addr(struct chip8 *vm, uint16_t instruction);

void run_call_addr(struct chip8 *vm, uint16_t instruction);

void run_se_vx_byte(struct chip8 *vm, uint16_t instruction);

void run_sne_vx_byte(struct chip8 *vm, uint16_t instruction);

void run_se_vx_vy(struct chip8 *vm, uint16_t instruction);

void run_ld_vx_byte(struct chip8 *vm, uint16_t instruction);

void run_add_vx_byte(struct chip8 *vm, uint16_t instruction);

void run_sne_vx_vy(struct chip8 *vm, uint16_t instruction);

void run_ld_i_addr(struct chip8 *vm, uint16_t instruction);

void run_ld_vx_vy(struct chip8 *vm, uint16_t instruction);

void run_or_vx_vy(struct chip8 *vm, uint16_t instruction);

void run_and_vx_vy(struct chip8 *vm, uint16_t instruction);

void run_xor_vx_vy(struct chip8 *vm, uint16_t instruction);

void run_add_vx_vy(struct chip8 *vm, uint16_t instruction);

void run_sub_vx_vy(struct chip8 *vm, uint16_t instruction);

void run_shr_vx(struct chip8 *vm, uint16_t instruction);

void run_subn_vx_vy(struct chip8 *vm, uint16_t instruction);

void run_shl_vx(struct chip8 *vm, uint16_t instruction);

void run_jp_v0_addr(struct chip8 *vm, uint16_t instruction);

void run_rnd_vx_byte(struct chip8 *vm, uint16_t instruction);

void run_drw_vx_vy_n(struct chip8 *vm, uint16_t instruction);

void run_ld_f_vx(struct chip8 *vm, uint16_t instruction);

void run_ld_b_vx(struct chip8 *vm, uint16_t instruction);

#endif
