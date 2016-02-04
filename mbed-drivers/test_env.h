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

#ifndef TEST_ENV_H_
#define TEST_ENV_H_

#include <stdio.h>
#include "mbed.h"

#define NL "\n"
#define RCNL "\r\n"

// Generic test suite transport protocol keys
extern const char* TEST_ENV_END;
extern const char* TEST_ENV_EXIT;
extern const char* TEST_ENV_SYNC;
extern const char* TEST_ENV_TIMEOUT;
extern const char* TEST_ENV_HOST_TEST_NAME;
// Test suite success code strings
extern const char* TEST_ENV_SUCCESS;
extern const char* TEST_ENV_FAILURE;
// Test case transport protocol start/finish keys
extern const char* TEST_ENV_TESTCASE_START;
extern const char* TEST_ENV_TESTCASE_FINISH;
// Code Coverage (LCOV)  transport protocol keys
extern const char* TEST_ENV_LCOV_START;
extern const char* TEST_ENV_LCOV_END;

// Test suite result related notification API
void greentea_send_kv(const char *, const char *);
void greentea_send_kv(const char *, const int);
void notify_start();
void notify_timeout(const int);
void notify_hosttest(const char *);
void notify_completion(const int);
void notify_testcase_start(const char *);
void notify_testcase_finish(const char *, const int);

// scanf replacement tokenizer and simple KV grammar parser
int greentea_parse_kv(char *, char *, const int, const int);


#ifdef YOTTA_CFG_DEBUG_OPTIONS_COVERAGE
// Code Coverage API
void notify_coverage_start(const char *path);
void notify_coverage_end();
#endif


// Test suite Host Test auto-detection macros

#define GREENTEA_START()                            notify_start();
#define GREENTEA_SETUP(TIMEOUT, HOST_TEST_NAME)     notify_timeout(TIMEOUT); notify_hosttest(HOST_TEST_NAME);
#define GREENTEA_SEND_KV(KEY,VALUE)                 greentea_send_kv(KEY,VALUE);

#define GREENTEA_TSUITE_RESULT(RESULT)              notify_completion(RESULT);
#define GREENTEA_TCASE_START(TESTCASE_UD)           notify_testcase_start(TESTCASE_UD);
#define GREENTEA_TCASE_FINISH(TESTCASE_UD,SUCCESS)  notify_testcase_finish(TESTCASE_UD,SUCCESS);

#endif
