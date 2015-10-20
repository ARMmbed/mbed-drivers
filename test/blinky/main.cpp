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

DigitalOut myled(LED1);
uint32_t count = 20;

void blink() {
    if (count) {
    	if (count == 1) {
    		MBED_HOSTTEST_RESULT(true);
    	}
    	count--;
    }
    myled = !myled;
}

void app_start(int, char*[]) {
    MBED_HOSTTEST_TIMEOUT(20);
    MBED_HOSTTEST_SELECT(default_auto);
    MBED_HOSTTEST_DESCRIPTION(Blinky);
    MBED_HOSTTEST_START("MBED_BLINKY");
    minar::Scheduler::postCallback(&blink).period(minar::milliseconds(200));
}
