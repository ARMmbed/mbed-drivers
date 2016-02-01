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

// Generic test suite transport protocol keys
const char* TEST_ENV_START = "start";
const char* TEST_ENV_END = "end";
const char* TEST_ENV_EXIT = "exit";
const char* TEST_ENV_SYNC = "sync";
const char* TEST_ENV_TIMEOUT = "timeout";
const char* TEST_ENV_HOST_TEST_NAME = "host_test_name";
// Test suite success code strings
const char* TEST_ENV_SUCCESS = "success";
const char* TEST_ENV_FAILURE = "failure";
// Test case transport protocol start/finish keys
const char* TEST_ENV_TESTCASE_START = "testcase_start";
const char* TEST_ENV_TESTCASE_FINISH = "testcase_finish";
// Code Coverage (LCOV)  transport protocol keys
const char* TEST_ENV_LCOV_START = "coverage_start";

// LCOV support
extern "C"
void gcov_exit(void);
#ifdef YOTTA_CFG_DEBUG_OPTIONS_COVERAGE
bool coverage_report = false;

void notify_coverage_start(const char *path) {
    printf("{{%s;%s;", TEST_ENV_LCOV_START, path);
}

void notify_coverage_end() {
    printf("}}" NL);
}

// notify_coverage_start() PAYLOAD notify_coverage_end()
// ((coverage_start;path;PAYLOAD}}

#endif

void notify_kv(const char *key, const char *val) {
    if (key && val) {
        printf("{{%s;%s}}" NL, key, val);
    }
}

void notify_kv(const char *key, const int val) {
    if (key) {
        printf("{{%s;%d}}" NL, key, val);
    }
}

void notify_kv(const char *key, const char *val, const int success) {
    if (key) {
        printf("{{%s;%s;%d}}" NL, key, val, success);
    }
}

void notify_kv(const char *key) {
    if (key) {
        printf("{{%s;%d}}" NL, key, 0);
    }
}

void notify_start() {
    // Sync preamble: "{{sync;0dad4a9d-59a3-4aec-810d-d5fb09d852c1}}"
    // Example value of sync_uuid == "0dad4a9d-59a3-4aec-810d-d5fb09d852c1"
    static char sync_uuid[48] = {0};
    scanf("{{sync;%47s}}", sync_uuid);  // Note: '47' in %s formatting!
    notify_kv(TEST_ENV_SYNC, sync_uuid);
}

void notify_timeout(const int timeout) {
    notify_kv(TEST_ENV_TIMEOUT, timeout);
}

void notify_hosttest(const char *host_test_name) {
    notify_kv(TEST_ENV_HOST_TEST_NAME, host_test_name);
}

void notify_completion(const int success) {
    const char *val = success ? TEST_ENV_SUCCESS : TEST_ENV_FAILURE;
    notify_kv(TEST_ENV_END, val);
#ifdef YOTTA_CFG_DEBUG_OPTIONS_COVERAGE
    coverage_report = true;
    gcov_exit();
    coverage_report = false;
#endif
    notify_kv(TEST_ENV_EXIT, !success);
}

// Test Case support

/** \brief Notifies test case start
  * \param Test Case ID name
  *
  * This function notifies test environment abort test case execution start.
  *
  */
void notify_testcase_start(const char *testcase_id) {
    notify_kv(TEST_ENV_TESTCASE_START, testcase_id);
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
void notify_testcase_finish(const char *testcase_id, const int success) {
    notify_kv(TEST_ENV_TESTCASE_FINISH, testcase_id, success);
}
