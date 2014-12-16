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
#include "mbed.h"
#include "test_env.h"

#if defined(TARGET_KL25Z)
SPI spi(PTD2, PTD3, PTD1);   // mosi, miso, sclk
DigitalOut cs(PTA13);
#elif defined(TARGET_KL05Z)
SPI spi(PTA7, PTA6, PTB0);   // mosi, miso, sclk
DigitalOut cs(PTB1);
#elif defined(TARGET_KL46Z)
SPI spi(PTD2, PTD3, PTD1);   // mosi, miso, sclk
DigitalOut cs(PTA13);
#elif defined(TARGET_FF_ARDUINO)
SPI spi(D11, D12, D13);   // mosi, miso, sclk
DigitalOut cs(D10);
#elif defined(TARGET_LIKE_STK3700)
SPI spi(PD0, PD1, PD2);
DigitalOut cs(PD3);
#else
SPI spi(p5, p6, p7); // mosi, miso, sclk
DigitalOut cs(p8);
#endif

int main() {
    int data = 0;
    int res = 0;

    for(int i = 0; i < 30; i++) {

        cs = 0;
        res = spi.write(data++);
        cs = 1;

        wait_ms(0.001);

        if ((i > 1) && ((res + 2) != data))
            notify_completion(false);
    }

    notify_completion(true);
}
