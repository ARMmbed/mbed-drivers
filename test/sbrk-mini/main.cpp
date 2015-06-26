#include <stddef.h>
#include <stdint.h>
#include "mbed/test_env.h"
#include "mbed/sbrk.h"

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
        ptr = (uintptr_t) _sbrk(TEST_SMALL);
        if (ptr != (uintptr_t) &SBRK_START) {
            break;
        }
        ptr = (uintptr_t) _sbrk(TEST_SMALL);
        if (ptr != (uintptr_t) &SBRK_START + TEST_SMALL) {
            break;
        }
        ptr = (uintptr_t) krbs(TEST_SMALL);
        if (ptr != (uintptr_t) &KRBS_START - TEST_SMALL) {
            break;
        }
        if (sbrk_diff != (ptrdiff_t)&__heap_size - 3*TEST_SMALL) {
            break;
        }

        tests_pass = true;
        // Test small increments
        for (unsigned int i = 0; tests_pass && i < TEST_SMALL; i++) {
            ptr = (uintptr_t) krbs(i);
            if (ptr & (TEST_SMALL - 1)) {
                tests_pass = false;
            }
        }
        if (!tests_pass) {
            break;
        }
        for (unsigned int i = 0; tests_pass && i < TEST_SMALL; i++) {
            ptr = (uintptr_t) _sbrk(i);
            if ((uintptr_t) sbrk_ptr & (TEST_SMALL - 1)) {
                tests_pass = false;
            }
        }
        if (!tests_pass) {
            break;
        }
        tests_pass = false;

        // Allocate a big block
        ptr = (uintptr_t) _sbrk((ptrdiff_t)&__heap_size);
        if (ptr != (uintptr_t) -1) {
            break;
        }
        ptr = (uintptr_t) krbs((ptrdiff_t)&__heap_size);
        if (ptr != (uintptr_t) -1) {
            break;
        }

        tests_pass = true;

    } while (0);

    MBED_HOSTTEST_RESULT(tests_pass);
    return !tests_pass;

}
