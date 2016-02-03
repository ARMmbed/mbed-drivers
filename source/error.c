/*
 * Copyright (c) 2006-2016, ARM Limited, All Rights Reserved
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
#include <stdlib.h>
#include <stdarg.h>
#include "device.h"
#include "compiler-polyfill/attributes.h"
#include "mbed-drivers/mbed_error.h"
#if DEVICE_STDIO_MESSAGES
#include <stdio.h>
#endif

__weak void error(const char* format, ...) {
#if DEVICE_STDIO_MESSAGES
    va_list arg;
    va_start(arg, format);
    vfprintf(stderr, format, arg);
    va_end(arg);
#endif
    exit(1);
}
