/*
 * Copyright (c) 2013-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed-drivers/mbed.h"
#include "greentea-client/test_env.h"
#include "utest/utest.h"

using namespace utest::v1;

Ticker tick;
DigitalOut led(LED1);

namespace {
    const int MS_INTERVALS = 1000;
}

void send_kv_tick() {
    static int count = 0;
    if (count < 10) {
        greentea_send_kv("tick", count);
    } else if (count == 10) {
        Harness::validate_callback();
    }
    count++;
}

void togglePin(void) {
    static int ticker_count = 0;
    if (ticker_count >= MS_INTERVALS) {
        send_kv_tick();
        ticker_count = 0;
        led = !led;
    }
    ticker_count++;
}

control_t test_case_ticker() {
    tick.attach_us(togglePin, 1000);
    return CaseTimeout(15 * 1000);
}

// Test cases
Case cases[] = {
    Case("Timers: 1x ticker", test_case_ticker),
};

status_t greentea_test_setup(const size_t number_of_cases) {
    GREENTEA_SETUP(20, "wait_us_auto");
    return greentea_test_setup_handler(number_of_cases);
}

Specification specification(greentea_test_setup, cases, greentea_test_teardown_handler);

void app_start(int, char*[]) {
    Harness::run(specification);
}
