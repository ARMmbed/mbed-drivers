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
#include "mbed-drivers/test_env.h"

// Const strings used in test_end
const char* TEST_ENV_START = "start";
const char* TEST_ENV_SUCCESS = "success";
const char* TEST_ENV_FAILURE = "failure";
const char* TEST_ENV_MEASURE = "measure";
const char* TEST_ENV_END = "end";

/* prototype */
#ifdef YOTTA_CFG_DEBUG_OPTIONS_COVERAGE
extern "C" void __gcov_flush();
extern bool coverage_report;    // In retarget.cpp
#endif

static void led_blink(PinName led, float delay)
{
    if (led != NC) {
        DigitalOut myled(led);
        while (1) {
            myled = !myled;
            wait(delay);
        }
    }
    while(1);
}

void notify_start()
{
    printf("{{%s}}" NL, TEST_ENV_START);
}

void notify_performance_coefficient(const char* measurement_name, const int value)
{
    printf("{{%s;%s;%d}}" RCNL, TEST_ENV_MEASURE, measurement_name, value);
}

void notify_performance_coefficient(const char* measurement_name, const unsigned int value)
{
    printf("{{%s;%s;%u}}" RCNL, TEST_ENV_MEASURE, measurement_name, value);
}

void notify_performance_coefficient(const char* measurement_name, const double value)
{
    printf("{{%s;%s;%f}}" RCNL, TEST_ENV_MEASURE, measurement_name, value);
}

void notify_completion(bool success)
{
    printf("{{%s}}" NL, success ? TEST_ENV_SUCCESS : TEST_ENV_FAILURE);
#ifdef YOTTA_CFG_DEBUG_OPTIONS_COVERAGE
    coverage_report = true;
    __gcov_flush();
    coverage_report = false;
#endif
    printf("{{%s}}" NL, TEST_ENV_END);
    led_blink(LED1, success ? 1.0 : 0.1);
}

bool notify_completion_str(bool success, char* buffer)
{
    bool result = false;
    if (buffer) {
        sprintf(buffer, "{{%s}}" NL "{{%s}}" NL, success ? TEST_ENV_SUCCESS : TEST_ENV_FAILURE, TEST_ENV_END);
        result = true;
    }
    return result;
}

// Host test auto-detection API
void notify_host_test_name(const char *host_test) {
    if (host_test) {
        printf("{{host_test_name;%s}}" NL, host_test);
    }
}

void notify_timeout(int timeout) {
    printf("{{timeout;%d}}" NL, timeout);
}

void notify_test_id(const char *test_id) {
    if (test_id) {
        printf("{{test_id;%s}}" NL, test_id);
    }
}

void notify_test_description(const char *description) {
    if (description) {
        printf("{{description;%s}}" NL, description);
    }
}

#ifdef YOTTA_CFG_DEBUG_OPTIONS_COVERAGE
void notify_coverage_start(const char *path) {
    printf("{{coverage_start;%s}}" NL, path);
}
void notify_coverage_end() {
    printf("{{coverage_end}}" NL);
}
#endif

// -DMBED_BUILD_TIMESTAMP=1406208182.13
unsigned int testenv_randseed()
{
    unsigned int seed = 0;
#ifdef MBED_BUILD_TIMESTAMP
    long long_seed = static_cast<long>(MBED_BUILD_TIMESTAMP);
    seed = long_seed & 0xFFFFFFFF;
#endif /* MBED_BUILD_TIMESTAMP */
    return seed;
}
