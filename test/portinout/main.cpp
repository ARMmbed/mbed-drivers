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


PortInOut port1(TEST_PortInOut_PORT0, TEST_PortInOut_MASK0);
PortInOut port2(TEST_PortInOut_PORT1, TEST_PortInOut_MASK1);

int main() {
    bool check = true;

    port1.output();
    port2.input();

    port1 = MASK_1; wait(0.1);
    if (port2 != MASK_2) check = false;

    port1 = 0; wait(0.1);
    if (port2 != 0) check = false;

    port1.input();
    port2.output();

    port2 = MASK_2; wait(0.1);
    if (port1 != MASK_1) check = false;

    port2 = 0; wait(0.1);
    if (port1 != 0) check = false;

    notify_completion(check);
}
