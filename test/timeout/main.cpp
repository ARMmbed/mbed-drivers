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

#include "mbed-drivers/mbed.h"
#include "mbed-drivers/test_env.h"

Timeout timer;
DigitalOut led(LED1);

namespace {
    const int MS_INTERVALS = 1000;
}

void print_char() {
    static int count = 0;
    if (count < 10) {
        greentea_send_kv("tick", count);
    } else if (count == 10) {
        GREENTEA_TESTSUITE_RESULT(true);
    }
    count++;
}

void toggleOff(void);

void toggleOn(void) {
    static int toggle_counter = 0;
    if (toggle_counter == MS_INTERVALS) {
        led = !led;
        print_char();
        toggle_counter = 0;
    }
    toggle_counter++;
    timer.attach_us(toggleOff, 500);
}

void toggleOff(void) {
    timer.attach_us(toggleOn, 500);
}

void app_start(int, char*[]) {
    GREENTEA_START();
    GREENTEA_SETUP(15, "wait_us_auto");

    toggleOn();
}
