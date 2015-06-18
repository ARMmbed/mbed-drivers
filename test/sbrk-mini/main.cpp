#include <stddef.h>
#include <stdint.h>
#include "mbed/test_env.h"


uint32_t SBRK_START;
uint32_t test_region[1024U];
uint32_t KRBS_START;

extern "C" void * sbrk(const size_t size);
extern "C" void * krbs(const size_t size);

extern volatile void * sbrk_ptr;
extern volatile uintptr_t sbrk_diff;

#define TEST_SMALL sizeof(uint32_t)

int main()
{
    MBED_HOSTTEST_TIMEOUT(10);
    MBED_HOSTTEST_SELECT(default);
    MBED_HOSTTEST_DESCRIPTION(sbrk mini test);
    MBED_HOSTTEST_START("SBRK_MINI_TEST");
    bool tests_pass = false;

    do {
        uintptr_t ptr;
        ptr = (uintptr_t) sbrk(TEST_SMALL);
        if (ptr != (uintptr_t) &SBRK_START) {
            break;
        }
        ptr = (uintptr_t) sbrk(TEST_SMALL);
        if (ptr != (uintptr_t) &SBRK_START + TEST_SMALL) {
            break;
        }
        ptr = (uintptr_t) krbs(TEST_SMALL);
        if (ptr != (uintptr_t) &KRBS_START - TEST_SMALL) {
            break;
        }
        if (sbrk_diff != sizeof(test_region) - 3*TEST_SMALL) {
            break;
        }

        tests_pass = true;
        // Test small increments
        for (uint i = 0; tests_pass && i < TEST_SMALL; i++) {
            ptr = (uintptr_t) krbs(i);
            if (ptr & (TEST_SMALL - 1)) {
                tests_pass = false;
            }
        }
        if (!tests_pass) {
            break;
        }
        for (uint i = 0; tests_pass && i < TEST_SMALL; i++) {
            ptr = (uintptr_t) sbrk(i);
            if ((uintptr_t) sbrk_ptr & (TEST_SMALL - 1)) {
                tests_pass = false;
            }
        }
        if (!tests_pass) {
            break;
        }
        tests_pass = false;

        // Allocate a big block
        ptr = (uintptr_t) sbrk(sizeof(test_region));
        if (ptr != (uintptr_t) -1) {
            break;
        }
        ptr = (uintptr_t) krbs(sizeof(test_region));
        if (ptr != (uintptr_t) NULL) {
            break;
        }

        tests_pass = true;

    } while (0);

    MBED_HOSTTEST_RESULT(tests_pass);
    return !tests_pass;

}
