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
#ifndef TEST_ENV_H_
#define TEST_ENV_H_

#warning mbed-drivers/test_env.h is deprecated. Please use greentea-client/test_env.h instead.

#include <stdio.h>
#include "mbed.h"

#define NL "\n"
#define RCNL "\r\n"

// Const strings used in test_end
extern const char* TEST_ENV_START;
extern const char* TEST_ENV_SUCCESS;
extern const char* TEST_ENV_FAILURE;
extern const char* TEST_ENV_MEASURE;
extern const char* TEST_ENV_END;

// Test result related notification functions
void notify_start();
void notify_completion(bool success);
bool notify_completion_str(bool success, char* buffer);
void notify_performance_coefficient(const char* measurement_name, const int value);
void notify_performance_coefficient(const char* measurement_name, const unsigned int value);
void notify_performance_coefficient(const char* measurement_name, const double value);

// Host test auto-detection API
void notify_host_test_name(const char *host_test);
void notify_timeout(int timeout);
void notify_test_id(const char *test_id);
void notify_test_description(const char *description);

// Code Coverage API
void notify_coverage_start(const char *path);
void notify_coverage_end();

// Host test auto-detection API
#define MBED_HOSTTEST_START(TESTID)      notify_test_id(TESTID); notify_start()
#define MBED_HOSTTEST_SELECT(NAME)       notify_host_test_name(#NAME)
#define MBED_HOSTTEST_TIMEOUT(SECONDS)   notify_timeout(SECONDS)
#define MBED_HOSTTEST_DESCRIPTION(DESC)  notify_test_description(#DESC)
#define MBED_HOSTTEST_RESULT(RESULT)     notify_completion(RESULT)
#define MBED_HOSTTEST_ASSERT(cond)       \
    do {                                 \
        if (!(cond)) {                   \
            printf("HOSTTEST ASSERTION FAILED: '%s' in %s, line %d\r\n", #cond, __FILE__, __LINE__); \
            notify_completion(false);    \
        }                                \
    } while(false)

/**
    Test auto-detection preamble example:
    main() {
        MBED_HOSTTEST_TIMEOUT(10);
        MBED_HOSTTEST_SELECT( host_test );
        MBED_HOSTTEST_DESCRIPTION(Hello World);
        MBED_HOSTTEST_START("MBED_10");
        // Proper 'host_test.py' should take over supervising of this test

        // Test code
        bool result = ...;

        MBED_HOSTTEST_RESULT(result);
    }
*/


// Test functionality useful during testing
unsigned int testenv_randseed();

// Macros, unit test like to provide basic comparisons
#define TESTENV_STRCMP(GIVEN,EXPECTED) (strcmp(GIVEN,EXPECTED) == 0)

// macros passed via test suite
#ifndef TEST_SUITE_TARGET_NAME
#define TEST_SUITE_TARGET_NAME "Unknown"
#endif

#ifndef TEST_SUITE_TEST_ID
#define TEST_SUITE_TEST_ID "Unknown"
#endif

#ifndef TEST_SUITE_UUID
#define TEST_SUITE_UUID "Unknown"
#endif

#endif
