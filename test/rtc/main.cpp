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

#define CUSTOM_TIME  1256729737

void app_start(int, char*[]) {
    // !!! FIXME: make this asynchronous

    MBED_HOSTTEST_TIMEOUT(20);
    MBED_HOSTTEST_SELECT(rtc_auto);
    MBED_HOSTTEST_DESCRIPTION(RTC);
    MBED_HOSTTEST_START("MBED_16");

    char buffer[32] = {0};
    set_time(CUSTOM_TIME);  // Set RTC time to Wed, 28 Oct 2009 11:35:37
    while(1) {
        time_t seconds = time(NULL);
        strftime(buffer, 32, "%Y-%m-%d %H:%M:%S %p", localtime(&seconds));
        printf("MBED: [%ld] [%s]\r\n", seconds, buffer);
        wait(1);
    }
}
