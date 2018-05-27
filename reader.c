#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define CLS 0x00E0
#define RET 0x00EE

#define print(c, name) printf("%x: %s\n", c, name)
#define REG_1(instr) (instr & 0x0F00) >> 8
#define REG_2(instr) (instr & 0x00F0) >> 4
#define LOW_BYTE(instr) instr & 0x00FF
#define MEM_ADDR(instr) instr & 0x0FFF
#define LOW_NIBBLE(instr) instr & 0x000F
#define HIGH_NIBBLE(instr) (instr & 0xF000) >> 12

void print_mem_instr(char *name, uint16_t instr) {
    printf("%x: %s %x\n", instr, name, MEM_ADDR(instr));
}

void print_single_reg_instr(char *name, uint16_t instr) {
    printf("%x: %s V%x\n", instr, name, REG_1(instr));
}

void print_single_reg_instr_with_operand(char *name, uint16_t instr) {
    uint16_t reg = REG_1(instr);
    uint16_t operand = LOW_BYTE(instr);
    printf("%x: %s V%x, %x\n", instr, name, reg, operand);
}

void print_two_reg_instr(char *name, uint16_t instr) {
    printf("%x: %s V%x, V%x\n", instr, name, REG_1(instr), REG_2(instr));
}

void print_two_reg_instr_with_operand(char *name, uint16_t instr) {
    uint16_t reg1 = REG_1(instr);
    uint16_t reg2 = REG_2(instr);
    uint16_t op = LOW_NIBBLE(instr);
    printf("%x: %s V%x, V%x, %x\n", instr, name, reg1, reg2, op);
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
            printf("%x: LD V%x, DT\n", instr, reg);
            break;
        case 0xA:
            printf("%x: LD V%x, K\n", instr, reg);
            break;
        case 0x15:
            printf("%x: LD V%x, DT\n", instr, reg);
            break;
        case 0x18:
            printf("%x: LD ST, V%x\n", instr, reg);
            break;
        case 0x1E:
            printf("%x: ADD I, V%x\n", instr, reg);
            break;
        case 0x29:
            printf("%x: LD F, V%x\n", instr, reg);
            break;
        case 0x33:
            printf("%x: LD B, V%x\n", instr, reg);
            break;
        case 0x55:
            printf("%x: LD [I], V%x\n", instr, reg);
            break;
        case 0x65:
            printf("%x: LD V%x, [I]\n", instr, reg);
            break;
        default:
            print(instr, "unknown");
            break;
    }
}

void print_instruction(uint16_t instr) {
    uint16_t instruction_type = HIGH_NIBBLE(instr);
    switch (instruction_type) {
        case 0:
            if (instr == CLS) {
                print(CLS, "CLS");
            } else if (instr == RET) {
                print(RET, "RET");
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
            print_mem_instr("LD I", instr);
            break;
        case 0xB:
            print_mem_instr("JP V0", instr);
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
                    print(instr, "unknown");
                    break;
            }
            break;
        case 0xF:
            break;
        default:
            print(instr, "unknown");
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

    uint16_t instr;

    while (fread(&instr, sizeof(instr), 1, fp)) {
        print_instruction(instr);
    }
}
