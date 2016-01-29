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

#include "mbed-drivers/test_env.h"

// Const strings used in test_end
const char* TEST_ENV_START = "start";
const char* TEST_ENV_SUCCESS = "success";
const char* TEST_ENV_FAILURE = "failure";
const char* TEST_ENV_MEASURE = "measure";
const char* TEST_ENV_END = "end";

/* prototype */
extern "C"
void gcov_exit(void);
#ifdef YOTTA_CFG_DEBUG_OPTIONS_COVERAGE
bool coverage_report = false;
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
    gcov_exit();
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

/** \brief Notifies test case start
  * \param Test Case ID name
  *
  * This function notifies test environment abort test case execution start.
  *
  */
void notify_testcase_start(const char *testcase_id)
{
    printf("{{testcase_start;%s}}" NL, testcase_id);
}

/** \brief Return partial (test case) result from test suite
  * \param Test Case ID name
  * \param Success code, 0 - success, >0 failure reason, <0 inconclusive
  *
  * This function passes partial test suite's test case result to test
  * environment.
  * Each test suite (in many cases one binary with tests) can return
  * multiple partial results used to track test case results.
  *
  * Test designers can use success code to return test case:
  * success == 0 - PASS, test case execution was successful.
  * success > 0  - FAILure, e.g. success == 404 can be used to
  *                pass "Server not found".
  * success < 0  - Inconclusive test case execution, e.g.
  *
  */
void notify_testcase_completion(const char *testcase_id, const int success)
{
    printf("{{testcase_finish;%s;%d}}" NL, testcase_id, success);
}
