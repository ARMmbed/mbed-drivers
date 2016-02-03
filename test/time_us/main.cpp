/*
 * Copyright (c) 2013-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed-drivers/mbed.h"
#include "mbed-drivers/test_env.h"

DigitalOut led(LED1);

namespace {
    const int MS_INTERVALS = 1000;
}

void print_char(char c = '*')
{
    printf("%c", c);
    fflush(stdout);
}


void app_start(int, char*[]) {
    // !!! FIXME: make this asynchronous

    MBED_HOSTTEST_TIMEOUT(15);
    MBED_HOSTTEST_SELECT(wait_us_auto);
    MBED_HOSTTEST_DESCRIPTION(Time us);
    MBED_HOSTTEST_START("MBED_25");

    while (true) {
        for (int i = 0; i < MS_INTERVALS; i++) {
            wait_us(1000);
        }
        led = !led; // Blink
        print_char();
    }
}
