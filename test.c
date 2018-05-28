#include <stdio.h>
#include "CuTest.h"
#include "machine.h"
#include "instructions.h"

void test_ld_reg_vx(CuTest* tc)
{
    struct chip8 vm;
    run_ld_vx_byte(&vm, 0x6a42);
    run_ld_vx_byte(&vm, 0x60F0);
    run_ld_vx_byte(&vm, 0x6789);
    run_ld_vx_byte(&vm, 0x6Fbb);
    CuAssertIntEquals(tc, vm.reg_v[0xA], 0x42);
    CuAssertIntEquals(tc, vm.reg_v[0x0], 0xF0);
    CuAssertIntEquals(tc, vm.reg_v[0x7], 0x89);
    CuAssertIntEquals(tc, vm.reg_v[0xF], 0xbb);
}

CuSuite* get_instruction_test_suite(void)
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, test_ld_reg_vx);

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
