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
#include "test_env.h"

Ticker tick;
DigitalOut led(TEST_PIN_LED1);

namespace {
    const int MS_INTERVALS = 1000;
}

class TickerHandler {
public:
    TickerHandler(const int _ms_intervals) : m_ticker_count(0), m_ms_intervals(_ms_intervals) {
    }

    void print_char(char c = '*')
    {
        printf("%c", c);
        fflush(stdout);
    }

    void togglePin(void)
    {
        if (ticker_count >= MS_INTERVALS) {
            print_char();
            ticker_count = 0;
            led = !led; // Blink
        }
        ticker_count++;
    }

protected:
    int m_ticker_count;
};

int main()
{
    TickerHandler th;

    tick.attach_us(th, TickerHandler::togglePin, 1000);
    while (1);
}
