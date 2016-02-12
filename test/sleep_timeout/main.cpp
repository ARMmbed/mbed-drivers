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
#include "greentea-client/test_env.h"
#include "minar/minar.h"

DigitalOut led1(LED1);
DigitalOut led2(LED2);
Timeout to1;
Timeout to2;
volatile int led1_initial;
volatile int led2_initial;

void check_leds() {
    bool result = true;

    result = result && ((int)led1 != led1_initial);
    result = result && ((int)led2 != led2_initial);

    GREENTEA_TESTSUITE_RESULT(result);
}

void led1_on() {
    led1 = !led1;
}

void led2_on() {
    led2 = !led2;
    minar::Scheduler::postCallback(mbed::util::FunctionPointer(check_leds).bind());
}

void app_start(int, char*[]) {
    GREENTEA_SETUP(5, "default_auto");

    led1 = 0;
    led2 = 0;
    led1_initial = led1;
    led2_initial = led2;
    to1.attach_us(led1_on, 1000000);
    to2.attach_us(led2_on, 2000000);
}
