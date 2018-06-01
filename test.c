#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include "CuTest.h"
#include "machine.h"
#include "instructions.h"

void test_ret(CuTest* tc) {
    struct chip8 vm = { .pc = 0x255, .sp = 1 };
    vm.stack[0] = 0x230;

    run_ret(&vm);
    CuAssertIntEquals(tc, 0x230, vm.pc);
    CuAssertIntEquals(tc, 0, vm.sp);
    CuAssertIntEquals(tc, 0, vm.error);
}

void test_ret_stack_underflow(CuTest *tc) {
    struct chip8 vm = { .pc = 0x255, .sp = 0 };

    run_ret(&vm);
    CuAssertIntEquals(tc, 0x255, vm.pc);
    CuAssertIntEquals(tc, 0, vm.sp);
    CuAssertIntEquals(tc, ERROR_STACK_UNDERFLOW, vm.error);
}

void test_jp_addr(CuTest* tc) {
    struct chip8 vm;

    run_jp_addr(&vm, 0x12d4);
    CuAssertIntEquals(tc, 0x2d4, vm.pc);

    run_jp_addr(&vm, 0x1FFF);
    CuAssertIntEquals(tc, 0xFFF, vm.pc);

    run_jp_addr(&vm, 0x1000);
    CuAssertIntEquals(tc, 0, vm.pc);
}

void test_call_addr(CuTest* tc) {
    struct chip8 vm = { .pc = 0x255 };

    run_call_addr(&vm, 0x22d4);
    CuAssertIntEquals(tc, 0, vm.error);
    CuAssertIntEquals(tc, 0x2d4, vm.pc);
    CuAssertIntEquals(tc, 1, vm.sp);
    CuAssertIntEquals(tc, 0x255, vm.stack[0]);
}

void test_se_vx_byte(CuTest* tc) {
    struct chip8 vm = { .pc = 0x220 };
    vm.reg_v[0] = 0x29;
    vm.reg_v[0xD] = 0x36;

    run_se_vx_byte(&vm, 0x3030);
    CuAssertIntEquals(tc, 0x220, vm.pc);

    run_se_vx_byte(&vm, 0x3029);
    CuAssertIntEquals(tc, 0x224, vm.pc);

    run_se_vx_byte(&vm, 0x3D42);
    CuAssertIntEquals(tc, 0x224, vm.pc);

    run_se_vx_byte(&vm, 0x3D36);
    CuAssertIntEquals(tc, 0x228, vm.pc);
}

void test_sne_vx_byte(CuTest* tc) {
    struct chip8 vm = { .pc = 0x220 };
    vm.reg_v[0] = 0x29;
    vm.reg_v[0xD] = 0x50;

    run_sne_vx_byte(&vm, 0x4029);
    CuAssertIntEquals(tc, 0x220, vm.pc);

    run_sne_vx_byte(&vm, 0x4030);
    CuAssertIntEquals(tc, 0x224, vm.pc);

    run_sne_vx_byte(&vm, 0x4D50);
    CuAssertIntEquals(tc, 0x224, vm.pc);

    run_sne_vx_byte(&vm, 0x4D10);
    CuAssertIntEquals(tc, 0x228, vm.pc);
}

void test_se_vx_vy(CuTest* tc) {
    struct chip8 vm = { .pc = 0x220 };
    vm.reg_v[0] = 0x29;
    vm.reg_v[5] = 0x42;
    vm.reg_v[0xA] = 0x29;

    run_se_vx_vy(&vm, 0x5050);
    CuAssertIntEquals(tc, 0x220, vm.pc);

    run_se_vx_vy(&vm, 0x50A0);
    CuAssertIntEquals(tc, 0x224, vm.pc);
}

void test_call_addr_stack_overflow(CuTest* tc) {
    struct chip8 vm = { .pc = 0x255, .sp = STACK_SIZE };

    run_call_addr(&vm, 0x22d4);
    CuAssertIntEquals(tc, 0x255, vm.pc);
    CuAssertIntEquals(tc, STACK_SIZE, vm.sp);
    CuAssertIntEquals(tc, ERROR_STACK_OVERFLOW, vm.error);
}

void test_ld_vx_byte(CuTest* tc) {
    struct chip8 vm;
    run_ld_vx_byte(&vm, 0x6A42);
    run_ld_vx_byte(&vm, 0x60F0);
    run_ld_vx_byte(&vm, 0x6789);
    run_ld_vx_byte(&vm, 0x6FBB);
    CuAssertIntEquals(tc, 0x42, vm.reg_v[0xA]);
    CuAssertIntEquals(tc, 0xF0, vm.reg_v[0x0]);
    CuAssertIntEquals(tc, 0x89, vm.reg_v[0x7]);
    CuAssertIntEquals(tc, 0xBB, vm.reg_v[0xF]);
}

void test_add_vx_byte(CuTest* tc) {
    struct chip8 vm;
    vm.reg_v[0xA] = 0x10;

    run_add_vx_byte(&vm, 0x7A02);
    CuAssertIntEquals(tc, 0x12, vm.reg_v[0xA]);

    run_add_vx_byte(&vm, 0x7AFF);
    CuAssertIntEquals(tc, 0x12, vm.reg_v[0xA]);
    CuAssertIntEquals(tc, ERROR_INTEGER_OVERFLOW, vm.error);
}

void test_sne_vx_vy(CuTest* tc) {
    struct chip8 vm = { .pc = 0x220 };
    vm.reg_v[0] = 0x29;
    vm.reg_v[5] = 0x42;
    vm.reg_v[0xA] = 0x29;

    run_sne_vx_vy(&vm, 0x9050);
    CuAssertIntEquals(tc, 0x224, vm.pc);

    run_sne_vx_vy(&vm, 0x90A0);
    CuAssertIntEquals(tc, 0x224, vm.pc);
}

void test_ld_i_addr(CuTest* tc) {
    struct chip8 vm;
    run_ld_i_addr(&vm, 0xAFFF);
    CuAssertIntEquals(tc, 0xFFF, vm.reg_i);

    run_ld_i_addr(&vm, 0xA000);
    CuAssertIntEquals(tc, 0x000, vm.reg_i);

    run_ld_i_addr(&vm, 0xA59B);
    CuAssertIntEquals(tc, 0x59B, vm.reg_i);
}

void test_ld_vx_vy(CuTest* tc) {
    struct chip8 vm;
    int i;
    for (i = 0; i < 16; ++i) {
        vm.reg_v[i] = i * 2;
    }

    run_ld_vx_vy(&vm, 0x8ab0);
    CuAssertIntEquals(tc, 2*0xB, vm.reg_v[0xA]);

    run_ld_vx_vy(&vm, 0x8F00);
    CuAssertIntEquals(tc, 0, vm.reg_v[0xF]);

    run_ld_vx_vy(&vm, 0x8050);
    CuAssertIntEquals(tc, 10, vm.reg_v[0]);

    run_ld_vx_vy(&vm, 0x8720);
    CuAssertIntEquals(tc, 4, vm.reg_v[7]);
}

void test_or_vx_vy(CuTest* tc) {
    struct chip8 vm;
    vm.reg_v[0xA] = 0b00110011;
    vm.reg_v[0xB] = 0b01010101;

    run_or_vx_vy(&vm, 0x8AB1);
    CuAssertIntEquals(tc, 0b01110111, vm.reg_v[0xA]);
    CuAssertIntEquals(tc, 0b01010101, vm.reg_v[0xB]);
}

void test_and_vx_vy(CuTest* tc) {
    struct chip8 vm;
    vm.reg_v[0xA] = 0b00001111;
    vm.reg_v[0xB] = 0b01010101;

    run_and_vx_vy(&vm, 0x8AB2);
    CuAssertIntEquals(tc, 0b00000101, vm.reg_v[0xA]);
    CuAssertIntEquals(tc, 0b01010101, vm.reg_v[0xB]);
}

void test_xor_vx_vy(CuTest* tc) {
    struct chip8 vm;
    vm.reg_v[0xA] = 0b11110000;
    vm.reg_v[0xB] = 0b01010101;

    run_xor_vx_vy(&vm, 0x8AB3);
    CuAssertIntEquals(tc, 0b10100101, vm.reg_v[0xA]);
    CuAssertIntEquals(tc, 0b01010101, vm.reg_v[0xB]);
}

void test_add_vx_vy(CuTest* tc) {
    struct chip8 vm;
    vm.reg_v[0xA] = 150;
    vm.reg_v[0xB] = 33;
    vm.reg_v[0xF] = 1;

    run_add_vx_vy(&vm, 0x8AB4);
    CuAssertIntEquals(tc, 183, vm.reg_v[0xA]);
    CuAssertIntEquals(tc, 0, vm.reg_v[0xF]);

    vm.reg_v[0xA] = 230;
    vm.reg_v[0xB] = 100;
    run_add_vx_vy(&vm, 0x8AB4);
    CuAssertIntEquals(tc, 0b01001010, vm.reg_v[0xA]);
    CuAssertIntEquals(tc, 1, vm.reg_v[0xF]);
}

void test_sub_vx_vy(CuTest* tc) {
    struct chip8 vm;
    vm.reg_v[0xA] = 150;
    vm.reg_v[0xB] = 30;

    run_sub_vx_vy(&vm, 0x8AB5);
    CuAssertIntEquals(tc, 120, vm.reg_v[0xA]);
    CuAssertIntEquals(tc, 1, vm.reg_v[0xF]);

    vm.reg_v[0xA] = 150;
    vm.reg_v[0xB] = 30;
    run_sub_vx_vy(&vm, 0x8BA5);
    CuAssertIntEquals(tc, (uint8_t) -120, vm.reg_v[0xB]);
    CuAssertIntEquals(tc, 0, vm.reg_v[0xF]);
}

void test_shr_vx(CuTest* tc) {
    struct chip8 vm;

    vm.reg_v[0xA] = 0b00110101;
    run_shr_vx(&vm, 0x8A06);
    CuAssertIntEquals(tc, 0b00011010, vm.reg_v[0xA]);
    CuAssertIntEquals(tc, 1, vm.reg_v[0xF]);

    vm.reg_v[0xA] = 0b11110010;
    run_shr_vx(&vm, 0x8A06);
    CuAssertIntEquals(tc, 0b01111001, vm.reg_v[0xA]);
    CuAssertIntEquals(tc, 0, vm.reg_v[0xF]);
}

void test_subn_vx_vy(CuTest* tc) {
    struct chip8 vm;
    vm.reg_v[0xA] = 150;
    vm.reg_v[0xB] = 30;

    run_subn_vx_vy(&vm, 0x8AB7);
    CuAssertIntEquals(tc, (uint8_t) -120, vm.reg_v[0xA]);
    CuAssertIntEquals(tc, 0, vm.reg_v[0xF]);

    vm.reg_v[0xA] = 150;
    vm.reg_v[0xB] = 30;
    run_subn_vx_vy(&vm, 0x8BA7);
    CuAssertIntEquals(tc, 120, vm.reg_v[0xB]);
    CuAssertIntEquals(tc, 1, vm.reg_v[0xF]);
}

void test_shl_vx(CuTest* tc) {
    struct chip8 vm;

    vm.reg_v[0xA] = 0b11110101;
    run_shl_vx(&vm, 0x8A0E);
    CuAssertIntEquals(tc, 0b11101010, vm.reg_v[0xA]);
    CuAssertIntEquals(tc, 1, vm.reg_v[0xF]);

    vm.reg_v[0xA] = 0b01011111;
    run_shl_vx(&vm, 0x8A0E);
    CuAssertIntEquals(tc, 0b10111110, vm.reg_v[0xA]);
    CuAssertIntEquals(tc, 0, vm.reg_v[0xF]);
}

void test_jp_v0_addr(CuTest* tc) {
    struct chip8 vm = { .pc = 0x200 };
    vm.reg_v[0] = 0x0EE;

    run_jp_v0_addr(&vm, 0xB311);
    CuAssertIntEquals(tc, 0x3FF, vm.pc);
}

void test_rnd_vx_byte(CuTest* tc) {
    srand(time(NULL));

    struct chip8 vm;

    bool pass = false;
    int rounds = 0;
    while (rounds < 20 && !pass) {
        run_rnd_vx_byte(&vm, 0xCA11);
        pass = vm.reg_v[0xA] > 0;
        ++rounds;
    }
    CuAssertTrue(tc, pass);

    rounds = 0;
    pass = true;
    while (rounds < 20 && pass) {
        run_rnd_vx_byte(&vm, 0xCA01);
        pass = (vm.reg_v[0xA] & 0xF0) == 0;
        ++rounds;
    }
    CuAssertTrue(tc, pass);

    rounds = 0;
    pass = true;
    while (rounds < 20 && pass) {
        run_rnd_vx_byte(&vm, 0xCA10);
        pass = (vm.reg_v[0xA] & 0x0F) == 0;
        ++rounds;
    }
    CuAssertTrue(tc, pass);

    rounds = 0;
    pass = true;
    while (rounds < 20 && pass) {
        run_rnd_vx_byte(&vm, 0xCA00);
        pass = vm.reg_v[0xA] == 0;
        ++rounds;
    }
    CuAssertTrue(tc, pass);
}

CuSuite* get_instruction_test_suite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_ret);
    SUITE_ADD_TEST(suite, test_ret_stack_underflow);
    SUITE_ADD_TEST(suite, test_jp_addr);
    SUITE_ADD_TEST(suite, test_call_addr);
    SUITE_ADD_TEST(suite, test_se_vx_byte);
    SUITE_ADD_TEST(suite, test_sne_vx_byte);
    SUITE_ADD_TEST(suite, test_se_vx_vy);
    SUITE_ADD_TEST(suite, test_call_addr_stack_overflow);
    SUITE_ADD_TEST(suite, test_ld_vx_byte);
    SUITE_ADD_TEST(suite, test_add_vx_byte);
    SUITE_ADD_TEST(suite, test_sne_vx_vy);
    SUITE_ADD_TEST(suite, test_ld_i_addr);
    SUITE_ADD_TEST(suite, test_ld_vx_vy);
    SUITE_ADD_TEST(suite, test_or_vx_vy);
    SUITE_ADD_TEST(suite, test_and_vx_vy);
    SUITE_ADD_TEST(suite, test_xor_vx_vy);
    SUITE_ADD_TEST(suite, test_add_vx_vy);
    SUITE_ADD_TEST(suite, test_sub_vx_vy);
    SUITE_ADD_TEST(suite, test_shr_vx);
    SUITE_ADD_TEST(suite, test_subn_vx_vy);
    SUITE_ADD_TEST(suite, test_shl_vx);
    SUITE_ADD_TEST(suite, test_jp_v0_addr);
    SUITE_ADD_TEST(suite, test_rnd_vx_byte);

    return suite;
}

void run_tests(void)
{
    CuString *output = CuStringNew();
    CuSuite* suite = get_instruction_test_suite();

    CuSuiteRun(suite);
    CuSuiteSummary(suite, output);
    CuSuiteDetails(suite, output);
    printf("%s\n", output->buffer);
}

int main(void)
{
    run_tests();
}
