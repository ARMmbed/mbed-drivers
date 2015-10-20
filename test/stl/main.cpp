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

#include <algorithm>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <math.h>

#define BUFFER_SIZE 128
#define TABLE_SIZE(TAB) (sizeof(TAB) / sizeof(TAB[0]))

#define NEGATIVE_INTEGERS -32768,-3214,-999,-100,-1,0,1,4231,999,4123,32760,99999
#define POSITIVE_INTEGERS 32768,3214,999,100,1,0,1,4231,999,4123,32760,99999
#define FLOATS  0.002,0.92430,15.91320,791.77368,6208.2,25719.4952,426815.982588,6429271.046,42468024.93,212006462.910
#define FLOATS_STR  "0.002","0.92430","15.91320","791.77368","6208.2","25719.4952","426815.982588","6429271.046","42468024.93","212006462.910"

template <class T, class F>
void BubbleSort(T& array, size_t array_size, F functor)
{
    bool flag = true;
    size_t numLength = array_size;
    for(size_t i = 1; (i <= numLength) && flag; i++) {
        flag = false;
        for (size_t j = 0; j < (numLength - 1); j++) {
            if (functor(array[j+1], array[j])) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j+1] = temp;
                flag = true;
            }
        }
    }
}

struct printInt {
    void operator()(int i) {
        printf("%d ", i);
    }
};

struct printFloat {
    void operator()(float f) {
        printf("%f ", f);
    }
};

struct printString {
    void operator()(const char* s) {
        printf("%s ", s);
    }
};

struct greaterAbs {
    bool operator()(int a, int b) {
        return abs(a) > abs(b);
    }
};


void runTest()
{
    int p_integers[] = {POSITIVE_INTEGERS};
    int n_integers[] = {NEGATIVE_INTEGERS};
    float floats[] = {FLOATS};
    bool result = true;

    MBED_HOSTTEST_TIMEOUT(10);
    MBED_HOSTTEST_SELECT(default_auto);
    MBED_HOSTTEST_DESCRIPTION(C++ STL);
    MBED_HOSTTEST_START("MBED_A3"); 

    {
        std::vector<int> v_pints(p_integers, p_integers + TABLE_SIZE(p_integers));
        bool equal_result = std::equal(v_pints.begin(), v_pints.end(), p_integers);
        result = result && equal_result;
        printf("[%s] Fill vector with data\r\n", equal_result ? "OK" : "FAIL");
    }

    {
        const char* floats_str[] = {FLOATS_STR};
        float floats_transform[TABLE_SIZE(floats_str)] = {0.0};
        std::transform(floats_str, floats_str + TABLE_SIZE(floats_str), floats_transform, atof);
        bool equal_result = std::equal(floats_transform, floats_transform + TABLE_SIZE(floats_transform), floats);
        result = result && equal_result;
        printf("[%s] Transform float strings\r\n", equal_result ? "OK" : "FAIL");

        std::for_each(floats_str, floats_str + TABLE_SIZE(floats_str), printString());
        printf("\r\n");
        std::for_each(floats_transform, floats_transform + TABLE_SIZE(floats_transform), printFloat());
        printf("\r\n");
    }

    {
        std::vector<int> v_nints_1(n_integers, n_integers + TABLE_SIZE(n_integers));
        std::vector<int> v_nints_2(n_integers, n_integers + TABLE_SIZE(n_integers));
        {
            BubbleSort(v_nints_1, v_nints_1.size(), std::greater<int>());
            std::sort(v_nints_2.begin(), v_nints_2.end(), std::greater<int>());
            bool equal_result = std::equal(v_nints_1.begin(), v_nints_1.end(), v_nints_2.begin());
            result = result && equal_result;
            printf("[%s] Sort integers\r\n", equal_result ? "OK" : "FAIL");

            std::for_each(v_nints_1.begin(), v_nints_1.end(), printInt());
            printf("\r\n");
            std::for_each(v_nints_2.begin(), v_nints_2.end(), printInt());
            printf("\r\n");
        }

        {
            BubbleSort(v_nints_1, v_nints_1.size(), greaterAbs());
            std::sort(v_nints_2.begin(), v_nints_2.end(), greaterAbs());
            bool equal_result = std::equal(v_nints_1.begin(), v_nints_1.end(), v_nints_2.begin());
            result = result && equal_result;
            printf("[%s] Sort integers\r\n", equal_result ? "OK" : "FAIL");

            std::for_each(v_nints_1.begin(), v_nints_1.end(), printInt());
            printf("\r\n");
            std::for_each(v_nints_2.begin(), v_nints_2.end(), printInt());
            printf("\r\n");
        }
    }

    MBED_HOSTTEST_RESULT(result);
}

void app_start(int, char*[]) {
    // run the test both at init time and properly from the scheduler, the
    // standard library should work from both contexts
    runTest();
    minar::Scheduler::postCallback(&runTest);
}
