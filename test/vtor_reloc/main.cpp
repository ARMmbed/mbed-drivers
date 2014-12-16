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

// Interrupt table relocation test, based on the 'interrupt_in' test
// It will test an interrupt pin before and after the interrupt table is relocated
// Works only on LPC1768

#include "test_env.h"
#include "cmsis_nvic.h"
#include <string.h>

#define PIN_IN      (p5)
#define PIN_OUT     (p25)
#define NUM_VECTORS (16+33)

DigitalOut out(PIN_OUT);
DigitalOut myled(LED1);

volatile int checks = 0;
uint32_t int_table[NUM_VECTORS];

#define FALLING_EDGE_COUNT 5

void flipper() {
    for (int i = 0; i < FALLING_EDGE_COUNT; i++) {
        out = 1;
        wait(0.2);
        out = 0;
        wait(0.2);
    }
}

void in_handler() {
    checks++;
    myled = !myled;
}

static bool test_once() {
    InterruptIn in(PIN_IN);
    checks = 0;
    printf("Interrupt table location: 0x%08X\r\n", SCB->VTOR);
    in.rise(NULL);
    in.fall(in_handler);
    flipper();
    in.fall(NULL);
    bool result = (checks == FALLING_EDGE_COUNT);
    printf("Falling edge checks counted: %d ... [%s]\r\n", checks, result ? "OK" : "FAIL");
    return result;
}

int main() {

    // First test, no table reallocation
    {
        printf("Starting first test (interrupts not relocated).\r\n");
        bool ret = test_once();
        if (ret == false) {
            notify_completion(false);
            return 1;
        }
    }

    // Relocate interrupt table and test again
    {
        printf("Starting second test (interrupts relocated).\r\n");
        memcpy(int_table, (void*)SCB->VTOR, sizeof(int_table));
        SCB->VTOR = (uint32_t)int_table;

        bool ret = test_once();
        if (ret == false) {
            notify_completion(false);
            return 1;
        }
    }

    notify_completion(true);
    return 0;
}
