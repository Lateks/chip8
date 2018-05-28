#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#define LOW_BYTE(instr) instr & 0x00FF
#define MEM_ADDR(instr) instr & 0x0FFF
#define HIGH_NIBBLE(instr) (instr & 0xF000) >> 12
#define REG_1(instr) (instr & 0x0F00) >> 8
#define REG_2(instr) (instr & 0x00F0) >> 4
#define LOW_NIBBLE(instr) instr & 0x000F

#endif
