/*
 * PackageLicenseDeclared: Apache-2.0
 * Copyright (c) 2015 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stddef.h>
#include <stdint.h>
#include "mbed/test_env.h"
#include "mbed/sbrk.h"

extern void * volatile mbed_sbrk_ptr;
extern volatile uintptr_t mbed_sbrk_diff;

#define TEST_SMALL sizeof(uint32_t)
#define CHECK_EQ(A,B,P,F,L)\
    ((A) == (B) ? 1 : ((P) = false, (F) = __FILE__, (L) = __LINE__, 0))
#define CHECK_NEQ(A,B,P,F,L)\
    ((A) != (B) ? 1 : ((P) = false, (F) = __FILE__, (L) = __LINE__, 0))


bool runTest(int * line, const char ** file) {
    bool tests_pass = true;

    do {
        uintptr_t init_sbrk_ptr = (uintptr_t)mbed_sbrk(0);
        uintptr_t ptr;
        ptr = (uintptr_t) mbed_sbrk(TEST_SMALL);
        if(!CHECK_EQ(ptr, (uintptr_t) init_sbrk_ptr, tests_pass, *file, *line)) {
            break;
        }

        ptr = (uintptr_t) mbed_sbrk(TEST_SMALL);
        if(!CHECK_EQ(ptr, init_sbrk_ptr + TEST_SMALL, tests_pass, *file, *line)) {
            break;
        }

        ptr = (uintptr_t) mbed_krbs(TEST_SMALL);
        if(!CHECK_EQ(ptr, (uintptr_t) &__mbed_krbs_start - TEST_SMALL, tests_pass, *file, *line)) {
            break;
        }
        if(!CHECK_EQ(mbed_sbrk_diff, (ptrdiff_t)&__heap_size - 3*TEST_SMALL - (init_sbrk_ptr - (uintptr_t)&__mbed_sbrk_start), tests_pass, *file, *line)) {
            break;
        }


        // Test small increments
        for (unsigned int i = 0; tests_pass && i < TEST_SMALL; i++) {
            ptr = (uintptr_t) mbed_krbs(i);
            if(!CHECK_EQ(0, ptr & (TEST_SMALL - 1), tests_pass, *file, *line)) {
                break;
            }
        }
        for (unsigned int i = 0; tests_pass && i < TEST_SMALL; i++) {
            ptr = (uintptr_t) mbed_sbrk(i);
            if(!CHECK_EQ(0, (uintptr_t) mbed_sbrk_ptr & (TEST_SMALL - 1), tests_pass, *file, *line)) {
                break;
            }
        }

        // Allocate a big block
        ptr = (uintptr_t) mbed_sbrk((ptrdiff_t)&__heap_size);
        if(!CHECK_EQ((intptr_t)ptr, -1, tests_pass, *file, *line)) {
            break;
        }

        ptr = (uintptr_t) mbed_krbs((ptrdiff_t)&__heap_size);
        if(!CHECK_EQ((intptr_t)ptr, -1, tests_pass, *file, *line)) {
            break;
        }

        break;

    } while (0);
    return tests_pass;
}

class Test {
public:
    Test():_pass(false), _line(0), _file(NULL)
    {
        _pass = runTest(&_line, &_file);
    }
    bool passed() {return _pass;}
    int line() {return _line;}
    const char * file() {return _file;}
private:
    bool _pass;
    int _line;
    const char *_file;
};

Test early_test;

void app_start(int, char*[])
{
    MBED_HOSTTEST_TIMEOUT(10);
    MBED_HOSTTEST_SELECT(default);
    MBED_HOSTTEST_DESCRIPTION(sbrk mini test);
    MBED_HOSTTEST_START("SBRK_MINI_TEST");

    if (!early_test.passed()) {
        printf("MBED: Failed at %s:%d\r\n", early_test.file(), early_test.line());
    }

    MBED_HOSTTEST_RESULT(early_test.passed());
    return;
}
