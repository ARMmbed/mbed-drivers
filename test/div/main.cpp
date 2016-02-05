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

#include <utility>      // std::pair
#include "mbed-drivers/mbed.h"
#include "mbed-drivers/test_env.h"

uint32_t test_64(uint64_t ticks) {
    ticks >>= 3; // divide by 8
    if (ticks > 0xFFFFFFFF) {
        ticks /= 3;
    } else {
        ticks = (ticks * 0x55555556) >> 32; // divide by 3
    }
    return (uint32_t)(0xFFFFFFFF & ticks);
}

const char *result_str(bool result) {
    return result ? "[OK]" : "[FAIL]";
}

void runTest() {
    GREENTEA_START();
    GREENTEA_SETUP(5, "default_auto");

    {   // 0xFFFFFFFF *  8 =  0x7fffffff8
        GREENTEA_TCASE_START("_DIV001");
        std::pair<uint32_t, uint64_t> values = std::make_pair(0x55555555, 0x7FFFFFFF8);
        uint32_t test_ret = test_64(values.second);
        bool test_res = values.first == test_ret;
        printf("64bit: 0x7FFFFFFF8: expected 0x%lX got 0x%lX ... %s\n",
            values.first,
            test_ret,
            result_str(test_res));
        GREENTEA_TCASE_FINISH("_DIV001", !test_res);
    }

    {   // 0xFFFFFFFF * 24 = 0x17ffffffe8
        GREENTEA_TCASE_START("_DIV002");
        std::pair<uint32_t, uint64_t> values = std::make_pair(0xFFFFFFFF, 0x17FFFFFFE8);
        uint32_t test_ret = test_64(values.second);
        bool test_res = values.first == test_ret;
        printf("64bit: 0x17FFFFFFE8: expected 0x%lX got 0x%lX ... %s\n",
            values.first,
            test_ret,
            result_str(test_res));
        GREENTEA_TCASE_FINISH("_DIV002", !test_res);
    }

    GREENTEA_TSUITE_RESULT(true);
}

void app_start(int, char*[]) {
    minar::Scheduler::postCallback(&runTest);
}
