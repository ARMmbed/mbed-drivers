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

#define PATTERN_CHECK_VALUE  0xF0F0ADAD

class Test {

private:
    const char* name;
    const unsigned pattern;

public:
    Test(const char* _name) : name(_name), pattern(PATTERN_CHECK_VALUE)  {
        print("init");
    }

    void print(const char *message) {
        printf("%s::%s\n", name, message);
    }

    bool check_init(void) {
        bool result = (pattern == PATTERN_CHECK_VALUE);
        print(result ? "check_init: OK" : "check_init: ERROR");
        return result;
    }

    void stack_test(void) {
        print("stack_test");
        Test t("Stack");
        t.hello();
    }

    void hello(void) {
        print("hello");
    }

    ~Test() {
        print("destroy");
    }
};

/* Check C++ startup initialisation */
Test s("Static");

/* EXPECTED OUTPUT:
*******************
Static::init
Static::stack_test
Stack::init
Stack::hello
Stack::destroy
Static::check_init: OK
Heap::init
Heap::hello
Heap::destroy
*******************/
void runTest (void) {
    MBED_HOSTTEST_TIMEOUT(10);
    MBED_HOSTTEST_SELECT(default_auto);
    MBED_HOSTTEST_DESCRIPTION(C++);
    MBED_HOSTTEST_START("MBED_12");

    bool result = true;
    for (;;)
    {
        // Global stack object simple test
        s.stack_test();
        if (s.check_init() == false)
        {
            result = false;
            break;
        }

        // Heap test object simple test
        Test *m = new Test("Heap");
        m->hello();

        if (m->check_init() == false)
        {
            result = false;
        }
        delete m;
        break;
    }

    MBED_HOSTTEST_RESULT(result);
}

void app_start(int, char*[]) {
    minar::Scheduler::postCallback(&runTest);
}

