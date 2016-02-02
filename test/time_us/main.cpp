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

DigitalOut led(LED1);

namespace {
    const int MS_INTERVALS = 1000;
}

void print_char() {
    static int count = 0;
    if (count < 10) {
        GREENTEA_SEND_KV("tick", count);
    } else if (count == 10) {
        GREENTEA_TSUITE_RESULT(true);
    }
    count++;
}

void app_start(int, char*[]) {
    // !!! FIXME: make this asynchronous
    GREENTEA_START();
    GREENTEA_SETUP(15, "wait_us_auto");

    while (true) {
        for (int i = 0; i < MS_INTERVALS; i++) {
            wait_us(1000);
        }
        led = !led; // Blink
        print_char();
    }
}
