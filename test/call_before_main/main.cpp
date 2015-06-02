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

namespace {
    bool mbed_main_called = false;
}

extern "C" void mbed_main() {
    printf("MBED: mbed_main() call before main()\r\n");
    mbed_main_called = true;
}

int main() {
    MBED_HOSTTEST_TIMEOUT(20);
    MBED_HOSTTEST_SELECT(default_auto);
    MBED_HOSTTEST_DESCRIPTION(Call function mbed_main before main);
    MBED_HOSTTEST_START("MBED_A21");

    printf("MBED: main() starts now!\r\n");

    MBED_HOSTTEST_RESULT(mbed_main_called);
}
