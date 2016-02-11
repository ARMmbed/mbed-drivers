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
extern const char* TEST_ENV_TESTCASE_COUNT;
extern const char* TEST_ENV_TESTCASE_START;
extern const char* TEST_ENV_TESTCASE_FINISH;
extern const char* TEST_ENV_TESTCASE_SUMMARY;
// Code Coverage (LCOV)  transport protocol keys
extern const char* TEST_ENV_LCOV_START;
extern const char* TEST_ENV_LCOV_END;

/**
 *  Greentea-client related API for communication with host side
 */

void GREENTEA_SETUP(const int, const char *);
void GREENTEA_TESTSUITE_RESULT(const int);

/**
 * Test suite result related notification API
 */
void greentea_send_kv(const char *, const char *);
void greentea_send_kv(const char *, const int);
void greentea_send_kv(const char *, const int, const int);
void greentea_send_kv(const char *, const char *, const int);
void greentea_send_kv(const char *, const char *, const int, const int);
int greentea_parse_kv(char *, char *, const int, const int);

#ifdef YOTTA_CFG_DEBUG_OPTIONS_COVERAGE
// Code Coverage API
void notify_coverage_start(const char *path);
void notify_coverage_end();
#endif

#endif
