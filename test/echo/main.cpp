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
    const int BUFFER_SIZE = 48;
    char buffer[BUFFER_SIZE] = {0};
}

void app_start(int, char*[]) {
    // !!! FIXME: make this asynchronous!

    MBED_HOSTTEST_TIMEOUT(20);
    MBED_HOSTTEST_SELECT(echo);
    MBED_HOSTTEST_DESCRIPTION(Serial Echo at 115200);
    MBED_HOSTTEST_START("MBED_A9");

    Serial pc(TEST_PIN_Serial_ECHO_TX, TEST_PIN_Serial_ECHO_RX);
    pc.baud(115200);

    while (1) {
        pc.gets(buffer, BUFFER_SIZE - 1);
        pc.printf("%s", buffer);
    }
}
