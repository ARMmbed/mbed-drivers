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

#include <cctype>
#include <cstdio>


namespace {
    char _key[8] = {0};
    char _value[128] = {0};
}

void app_start(int, char*[]) {
    // !!! FIXME: make this asynchronous!

    GREENTEA_SETUP(15, "echo");

    for (int i = 0 ; i< 10; ++i) {
        greentea_parse_kv(_key, _value, sizeof(_key), sizeof(_value));
        greentea_send_kv(_key, _value);
    }

    GREENTEA_TESTSUITE_RESULT(true);
}
