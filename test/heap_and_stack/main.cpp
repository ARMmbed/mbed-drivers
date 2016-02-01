/* mbed Microcontroller Library
 * Copyright (c) 2013-2014 ARM Limited
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
#include "mbed-drivers/test_env.h"

static char *initial_stack_p;
static char *initial_heap_p;

static char line[256];
static unsigned int iterations = 0;

void report_iterations(void) {
    unsigned int tot = (0x100 * iterations)*2;
    printf("\nAllocated (%d)Kb in (%u) iterations\n", tot/1024, iterations);
#if !defined(TOOLCHAIN_GCC_CR)
    // EA: This causes a crash when compiling with GCC_CR???
    printf("%.2f\n", ((float)(tot)/(float)(initial_stack_p - initial_heap_p))*100.);
#endif
#ifdef TOOLCHAIN_ARM
#ifndef __MICROLIB
    __heapvalid((__heapprt) fprintf, stdout, 1);
#endif
#endif
}

bool stack_test(char *latest_heap_pointer) {
    bool result = true;
    char stack_line[256];
    iterations++;

    sprintf(stack_line, "\nstack pointer: %p", &stack_line[255]);
    puts(stack_line);

    char *heap_pointer = (char*)malloc(0x100);

    if (heap_pointer == NULL) {
        int diff = (&stack_line[255] - latest_heap_pointer);
        if (diff > 0x200) {
            sprintf(stack_line, "\n[WARNING] Malloc failed to allocate memory too soon. There are (0x%x) free bytes", diff);
            report_iterations();
            puts(stack_line);
        } else {
            puts("\n[SUCCESS] Stack/Heap collision detected");
            report_iterations();
        }
        return false;
    } else {
        heap_pointer += 0x100;
        sprintf(line, "heap pointer: %p", heap_pointer);
        puts(line);
    }

    if ((&stack_line[255]) > heap_pointer) {
        stack_test(heap_pointer);
    } else {
        puts("\n[WARNING] The Stack/Heap collision was not detected");
        result = false;
        report_iterations();
    }

    return result;
}


void runTest(void) {
    char c;
    bool result = false;
    GREENTEA_START();
    GREENTEA_SETUP(5, "default_auto");

    initial_stack_p = &c;

    {
        GREENTEA_TCASE_START("_HEAP0001");
        initial_heap_p = (char*)malloc(1);
        if (initial_heap_p) {
            printf("Initial stack/heap geometry:\n");
            printf("   stack pointer:V %p\n", initial_stack_p);
            printf("   heap pointer :^ %p\n", initial_heap_p);
            initial_heap_p++;
            result = stack_test(initial_heap_p);
        } else {
            printf("Unable to malloc a single byte\n");
            result = false;
        }
        GREENTEA_TCASE_FINISH("_HEAP0001", !result);
    }

    GREENTEA_TSUITE_RESULT(true);
}

void app_start(int, char*[]) {
    minar::Scheduler::postCallback(&runTest);
}
