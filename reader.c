#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "instructions.h"
#include "machine.h"

#define print_unknown() puts("unknown instruction or byte of data");

void print_mem_instr(char *name, uint16_t instr) {
    printf("%s %x\n", name, MEM_ADDR(instr));
}

void print_mem_reg_instr(char *name, char *reg_name, uint16_t instr) {
    printf("%s %s, %x\n", name, reg_name, MEM_ADDR(instr));
}

void print_single_reg_instr(char *name, uint16_t instr) {
    printf("%s V%x\n", name, REG_1(instr));
}

void print_single_reg_instr_with_operand(char *name, uint16_t instr) {
    uint16_t reg = REG_1(instr);
    uint16_t operand = LOW_BYTE(instr);
    printf("%s V%x, %x\n", name, reg, operand);
}

void print_two_reg_instr(char *name, uint16_t instr) {
    printf("%s V%x, V%x\n", name, REG_1(instr), REG_2(instr));
}

void print_two_reg_instr_with_operand(char *name, uint16_t instr) {
    uint16_t reg1 = REG_1(instr);
    uint16_t reg2 = REG_2(instr);
    uint16_t op = LOW_NIBBLE(instr);
    printf("%s V%x, V%x, %x\n", name, reg1, reg2, op);
}

void print_two_reg_op(uint16_t instr) {
    static char *op_names[] = {
        "LD", "OR", "AND", "XOR", "ADD", "SUB", "SHR", "SUBN", "SHL"
    };
    uint16_t optype = LOW_NIBBLE(instr);
    print_two_reg_instr(op_names[optype], instr);
}

void print_f_instr(uint16_t instr) {
    uint16_t reg = REG_1(instr);
    switch (LOW_BYTE(instr)) {
        case 7:
            printf("LD V%x, DT\n", reg);
            break;
        case 0xA:
            printf("LD V%x, K\n", reg);
            break;
        case 0x15:
            printf("LD V%x, DT\n", reg);
            break;
        case 0x18:
            printf("LD ST, V%x\n", reg);
            break;
        case 0x1E:
            printf("ADD I, V%x\n", reg);
            break;
        case 0x29:
            printf("LD F, V%x\n", reg);
            break;
        case 0x33:
            printf("LD B, V%x\n", reg);
            break;
        case 0x55:
            printf("LD [I], V%x\n", reg);
            break;
        case 0x65:
            printf("LD V%x, [I]\n", reg);
            break;
        default:
            print_unknown();
            break;
    }
}

void print_instruction(uint16_t instr, uint16_t addr) {
    uint16_t instruction_type = HIGH_NIBBLE(instr);
    printf("(%04x) %04x: ", addr, instr);

    switch (instruction_type) {
        case 0:
            if (instr == CLS) {
                puts("CLS");
            } else if (instr == RET) {
                puts("RET");
            } else {
                print_mem_instr("SYS", instr);
            }
            break;
        case 1:
            print_mem_instr("JP", instr);
            break;
        case 2:
            print_mem_instr("CALL", instr);
            break;
        case 3:
            print_single_reg_instr_with_operand("SE", instr);
            break;
        case 4:
            print_single_reg_instr_with_operand("SNE", instr);
            break;
        case 5:
            print_two_reg_instr("SE", instr);
            break;
        case 6:
            print_single_reg_instr_with_operand("LD", instr);
            break;
        case 7:
            print_single_reg_instr_with_operand("ADD", instr);
            break;
        case 8:
            print_two_reg_op(instr);
            break;
        case 9:
            print_two_reg_instr("SNE", instr);
            break;
        case 0xA:
            print_mem_reg_instr("LD", "I", instr);
            break;
        case 0xB:
            print_mem_reg_instr("JP", "V0", instr);
            break;
        case 0xC:
            print_single_reg_instr_with_operand("RND", instr);
            break;
        case 0xD:
            print_two_reg_instr_with_operand("DRW", instr);
            break;
        case 0xE:
            switch (LOW_BYTE(instr)) {
                case 0x9E:
                    print_single_reg_instr("SKP", instr);
                    break;
                case 0xA1:
                    print_single_reg_instr("SKNP", instr);
                    break;
                default:
                    print_unknown();
                    break;
            }
            break;
        case 0xF:
            print_f_instr(instr);
            break;
        default:
            print_unknown();
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: reader <filename>\n");
        exit(0);
    }

    char *filename = argv[1];
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        printf("Cannot open file %s.\n", filename);
        exit(1);
    }

    uint8_t instr[2];

    uint16_t addr = PROG_MEM_START;
    while (fread(&instr, sizeof(uint8_t), 2, fp)) {
        print_instruction((instr[0] << 8) | instr[1], addr);
        addr += 2;
    }
}
