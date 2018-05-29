#include <stdio.h>
#include "CuTest.h"
#include "machine.h"
#include "instructions.h"

void test_jp_addr(CuTest* tc) {
    struct chip8 vm;

    run_jp_addr(&vm, 0x12d4);
    CuAssertIntEquals(tc, 0x2d4, vm.pc);

    run_jp_addr(&vm, 0x1FFF);
    CuAssertIntEquals(tc, 0xFFF, vm.pc);

    run_jp_addr(&vm, 0x1000);
    CuAssertIntEquals(tc, 0, vm.pc);
}

void test_ld_reg_vx(CuTest* tc) {
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

CuSuite* get_instruction_test_suite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_jp_addr);
    SUITE_ADD_TEST(suite, test_ld_reg_vx);
    SUITE_ADD_TEST(suite, test_ld_i_addr);
    SUITE_ADD_TEST(suite, test_ld_vx_vy);

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
