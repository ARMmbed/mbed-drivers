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

void print_char(char c = '*')
{
    printf("%c", c);
    fflush(stdout);
}

Ticker flipper_1;
DigitalOut led1(LED1);

void flip_1() {
    static int led1_state = 0;
    if (led1_state) {
        led1 = 0; led1_state = 0;
    } else {
        led1 = 1; led1_state = 1;
    }
    print_char();
}

Ticker flipper_2;
DigitalOut led2(LED2);

void flip_2() {
    static int led2_state = 0;
    if (led2_state) {
        led2 = 0; led2_state = 0;
    } else {
        led2 = 1; led2_state = 1;
    }
}

void app_start(int, char*[]) {
    MBED_HOSTTEST_TIMEOUT(15);
    MBED_HOSTTEST_SELECT(wait_us_auto);
    MBED_HOSTTEST_DESCRIPTION(Ticker Int);
    MBED_HOSTTEST_START("MBED_11");

    led1 = 0;
    led2 = 0;
    flipper_1.attach(&flip_1, 1.0); // the address of the function to be attached (flip) and the interval (1 second)
    flipper_2.attach(&flip_2, 2.0); // the address of the function to be attached (flip) and the interval (2 seconds)
}
