#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#define INSTR_MASK 0xF000
#define MEM_ADDR_MASK 0x0FFF
#define REG_1_MASK 0x0F00
#define REG_2_MASK 0x00F0
#define TWO_REG_OPTYPE_MASK 0x000F
#define LOW_BYTE_MASK 0x00FF

#define CLS 0x00E0
#define RET 0x00EE
#define print(c, name) printf("%x: %s\n", c, name)

void print_mem_instr(char *name, uint16_t instr) {
    uint16_t mem_addr = instr & MEM_ADDR_MASK;
    printf("%x: %s %x\n", instr, name, mem_addr);
}

void print_single_reg_instr(char *name, uint16_t instr) {
    uint16_t reg = (instr & REG_1_MASK) >> 8;
    printf("%x: %s V%x\n", instr, name, reg);
}

void print_single_reg_instr_with_operand(char *name, uint16_t instr) {
    uint16_t reg = (instr & REG_1_MASK) >> 8;
    uint16_t operand = instr & LOW_BYTE_MASK;
    printf("%x: %s V%x, %x\n", instr, name, reg, operand);
}

void print_two_reg_instr(char *name, uint16_t instr) {
    uint16_t reg1 = (instr & REG_1_MASK) >> 8;
    uint16_t reg2 = (instr & REG_2_MASK) >> 4;
    printf("%x: %s V%x, V%x\n", instr, name, reg1, reg2);
}

void print_two_reg_instr_with_operand(char *name, uint16_t instr) {
    uint16_t reg1 = (instr & REG_1_MASK) >> 8;
    uint16_t reg2 = (instr & REG_2_MASK) >> 4;
    uint16_t op = instr & TWO_REG_OPTYPE_MASK;
    printf("%x: %s V%x, V%x, %x\n", instr, name, reg1, reg2, op);
}

void print_two_reg_op(uint16_t instr) {
    static char *op_names[] = {
        "LD", "OR", "AND", "XOR", "ADD", "SUB", "SHR", "SUBN", "SHL"
    };
    uint16_t optype = instr & TWO_REG_OPTYPE_MASK;
    print_two_reg_instr(op_names[optype], instr);
}

void print_f_instr(uint16_t instr) {
    uint16_t reg = (instr & REG_1_MASK) >> 8;
    switch (instr & LOW_BYTE_MASK) {
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
    switch ((instr & INSTR_MASK) >> 12) {
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
            switch (instr & LOW_BYTE_MASK) {
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
