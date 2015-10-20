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

void ticker_callback_1(void);
void ticker_callback_2(void);

DigitalOut led0(LED1);
DigitalOut led1(LED2);
Ticker ticker;

void print_char(char c = '*')
{
    printf("%c", c);
    fflush(stdout);
}

void ticker_callback_2(void)
{
    ticker.detach();
    ticker.attach(ticker_callback_1, 1.0);
    led1 = !led1;
    print_char();
}

void ticker_callback_1(void)
{
    ticker.detach();
    ticker.attach(ticker_callback_2, 1.0);
    led0 = !led0;
    print_char();
}

void app_start(int, char*[])
{
    MBED_HOSTTEST_TIMEOUT(15);
    MBED_HOSTTEST_SELECT(wait_us_auto);
    MBED_HOSTTEST_DESCRIPTION(Ticker Two callbacks);
    MBED_HOSTTEST_START("MBED_34");

    ticker.attach(ticker_callback_1, 1.0);
}
