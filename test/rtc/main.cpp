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

#define CUSTOM_TIME  1256729737

void app_start(int, char*[]) {
    // !!! FIXME: make this asynchronous
    GREENTEA_SETUP(15, "rtc_auto");
    greentea_send_kv("timestamp", CUSTOM_TIME);

    char buffer[32] = {0};
    char kv_buff[64] = {};
    set_time(CUSTOM_TIME);  // Set RTC time to Wed, 28 Oct 2009 11:35:37

    for (int i=0; i<10; ++i) {
        time_t seconds = time(NULL);
        sprintf(kv_buff, "[%ld] ", seconds);
        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S %p", localtime(&seconds));
        strcat(kv_buff, buffer);
        greentea_send_kv("rtc", kv_buff);
        wait(1);
    }

    GREENTEA_TESTSUITE_RESULT(true);
}
