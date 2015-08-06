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
#include "mbed.h"
#include "test_env.h"

BusOut bus1(TEST_PIN_BusOut_0, TEST_PIN_BusOut_1, TEST_PIN_BusOut_2, TEST_PIN_BusOut_3, TEST_PIN_BusOut_4, TEST_PIN_BusOut_5, TEST_PIN_BusOut_6, TEST_PIN_BusOut_7, TEST_PIN_BusOut_8, TEST_PIN_BusOut_9, TEST_PIN_BusOut_10, TEST_PIN_BusOut_11, TEST_PIN_BusOut_12, TEST_PIN_BusOut_13, TEST_PIN_BusOut_14, TEST_PIN_BusOut_15);
BusOut bus2(TEST_PIN_BusOut_16, TEST_PIN_BusOut_17, TEST_PIN_BusOut_18, TEST_PIN_BusOut_19, TEST_PIN_BusOut_20, TEST_PIN_BusOut_21);
int i;

int main()
{
    notify_start();
    
    for (i=0; i<=65535; i++) {
        bus1 = i;
        bus2 = i;
        wait(0.0001);
    }
    
    notify_completion(true);
}
