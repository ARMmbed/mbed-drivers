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
#include <string.h>

#define BUFFER_SIZE 256
#define CLEAN_BUFFER(BUFF) memset(BUFF, 0x00, BUFFER_SIZE)

#define NEGATIVE_INTEGERS -32768,-3214,-999,-100,-1,0,-1,-4231,-999,-4123,-32760,-99999
#define POSITIVE_INTEGERS 32768,3214,999,100,1,0,1,4231,999,4123,32760,99999
#define FLOATS  0.002,0.92430,15.91320,791.77368,6208.2,25719.4952,426815.982588,6429271.046,42468024.93,212006462.910


void runTest()
{
    char buffer[BUFFER_SIZE] = {0};
    bool result = true;
    bool cmp_result;

    MBED_HOSTTEST_TIMEOUT(10);
    MBED_HOSTTEST_SELECT(default_auto);
    MBED_HOSTTEST_DESCRIPTION(C string operations);
    MBED_HOSTTEST_START("MBED_33");
    {
        CLEAN_BUFFER(buffer);
        sprintf(buffer, "%i %d %i %d %i %d %i %d %i %d %i %i", NEGATIVE_INTEGERS);
        cmp_result = TESTENV_STRCMP(buffer, "-32768 -3214 -999 -100 -1 0 -1 -4231 -999 -4123 -32760 -99999");
        printf("[%s] %s\r\n", cmp_result ? "OK" : "FAIL", buffer);
        result = result && cmp_result;
    }

    {
        CLEAN_BUFFER(buffer);
        sprintf(buffer, "%u %d %u %d %u %d %u %d %u %d %u %d", POSITIVE_INTEGERS);
        cmp_result = TESTENV_STRCMP(buffer, "32768 3214 999 100 1 0 1 4231 999 4123 32760 99999");
        printf("[%s] %s\r\n", cmp_result ? "OK" : "FAIL", buffer);
        result = result && cmp_result;
    }

    {
        CLEAN_BUFFER(buffer);
        sprintf(buffer, "%x %X %x %X %x %X %x %X %x %X %x %X", POSITIVE_INTEGERS);
        cmp_result = TESTENV_STRCMP(buffer, "8000 C8E 3e7 64 1 0 1 1087 3e7 101B 7ff8 1869F");
        printf("[%s] %s\r\n", cmp_result ? "OK" : "FAIL", buffer);
        result = result && cmp_result;
    }

    {
        CLEAN_BUFFER(buffer);
        sprintf(buffer, "%f %f %f %f %f %f %f %f %f %f", FLOATS);
        cmp_result = TESTENV_STRCMP(buffer, "0.002000 0.924300 15.913200 791.773680 6208.200000 25719.495200 426815.982588 6429271.046000 42468024.930000 212006462.910000");
        printf("[%s] %s\r\n", cmp_result ? "OK" : "FAIL", buffer);
        result = result && cmp_result;
    }

    {
        CLEAN_BUFFER(buffer);
        sprintf(buffer, "%g %g %g %g %g %g %g %g %g %g", FLOATS);
        cmp_result = TESTENV_STRCMP(buffer, "0.002 0.9243 15.9132 791.774 6208.2 25719.5 426816 6.42927e+006 4.2468e+007 2.12006e+008");
        cmp_result = cmp_result || TESTENV_STRCMP(buffer, "0.002 0.9243 15.9132 791.774 6208.2 25719.5 426816 6.42927e+06 4.2468e+07 2.12006e+08");
        printf("[%s] %s\r\n", cmp_result ? "OK" : "FAIL", buffer);
        result = result && cmp_result;
    }

    {
        CLEAN_BUFFER(buffer);
        sprintf(buffer, "%e %E %e %E %e %E %e %E %e %E", FLOATS);
        cmp_result = TESTENV_STRCMP(buffer, "2.000000e-003 9.243000E-001 1.591320e+001 7.917737E+002 6.208200e+003 2.571950E+004 4.268160e+005 6.429271E+006 4.246802e+007 2.120065E+008");
        cmp_result = cmp_result || TESTENV_STRCMP(buffer, "2.000000e-03 9.243000E-01 1.591320e+01 7.917737E+02 6.208200e+03 2.571950E+04 4.268160e+05 6.429271E+06 4.246802e+07 2.120065E+08");
        printf("[%s] %s\r\n", cmp_result ? "OK" : "FAIL", buffer);
        result = result && cmp_result;
    }

    {
        CLEAN_BUFFER(buffer);
        char str[] ="- This, a sample string.";
        char * pch = strtok (str," ,.-");
        while (pch != NULL) {
            strcat(buffer, pch);
            pch = strtok (NULL, " ,.-");
        }
        cmp_result = TESTENV_STRCMP(buffer, "Thisasamplestring");
        printf("[%s] %s\r\n", cmp_result ? "OK" : "FAIL", buffer);
        result = result && cmp_result;
    }

    {
        CLEAN_BUFFER(buffer);
        char str[] = "This is a sample string";
        char key[] = "aeiou";
        char *pch = strpbrk(str, key);
        while (pch != NULL)
        {
            char buf[2] = {*pch, '\0'};
            strcat(buffer, buf);
            pch = strpbrk(pch + 1,key);
        }
        cmp_result = TESTENV_STRCMP(buffer, "iiaaei");
        printf("[%s] %s\r\n", cmp_result ? "OK" : "FAIL", buffer);
        result = result && cmp_result;
    }

    MBED_HOSTTEST_RESULT(result);
    return;
}

void app_start(int, char*[]) {
    minar::Scheduler::postCallback(&runTest);
}

