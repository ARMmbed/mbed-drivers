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
#include "test_env.h"

DigitalOut led1(TEST_PIN_LED1);
DigitalOut led2(TEST_PIN_LED2);
Timeout to1;
Timeout to2;

void led1_on() {
    led1 = !led1;
    printf("led1\n\r");
    fflush(stdout);
}

void led2_on() {
    led2 = !led2;
    printf("led2\n\r");
    fflush(stdout);
}

void app_start(int, char*[]) {
    led1 = 0;
    led2 = 0;
    to1.attach_us(led1_on, 1000000);
    to2.attach_us(led2_on, 2000000);
}
