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

class DevNull : public Stream {
public:
    DevNull(const char *name = NULL) : Stream(name) {}

protected:
    virtual int _getc() {
        return 0;
    }
    virtual int _putc(int c) {
        return c;
    }
};

DevNull null("null");

void runTest() {
    MBED_HOSTTEST_TIMEOUT(20);
    MBED_HOSTTEST_SELECT(dev_null_auto);
    MBED_HOSTTEST_DESCRIPTION(stdout redirected to dev null);
    MBED_HOSTTEST_START("EXAMPLE_1");
    
    printf("MBED: re-routing stdout to /null\r\n");
    freopen("/null", "w", stdout);
    printf("MBED: printf redirected to /null\r\n");   // This shouldn't appear
    // If failure message can be seen test should fail :)
    MBED_HOSTTEST_RESULT(false);   // This is 'false' on purpose
}

void app_start(int, char*[]) {
    minar::Scheduler::postCallback(&runTest);
}
