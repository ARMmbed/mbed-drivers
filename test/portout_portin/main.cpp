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

PortOut port_out(TEST_PortInOut_PORT0, TEST_PortInOut_MASK0);
PortIn  port_in (TEST_PortInOut_PORT1, TEST_PortInOut_MASK1);

int main() {
    port_out = MASK_1;
    wait(0.1);
    int value = port_in.read();
    if (value != MASK_2) {
        printf("[Test high] expected (0x%x) received (0x%x)\n", MASK_2, value);
        notify_completion(false);
    }

    port_out = 0;
    wait(0.1);
    value = port_in.read();
    if (value != 0) {
        printf("[Test low] expected (0x%x) received (0x%x)\n", 0, value);
        notify_completion(false);
    }

    notify_completion(true);
}
