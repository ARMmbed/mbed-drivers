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

#if defined(TARGET_LIKE_STK3700)
SPI spi(PD0, PD1, PD2);
DigitalOut latchpin(PD3);

#else // defined(TARGET_LIKE_STK3700)
SPI spi(p11, p12, p13);
DigitalOut latchpin(p10);
#endif // else defined(TARGET_LIKE_STK3700)

int main() {
    spi.format(8, 0);
    spi.frequency(16 * 1000 * 1000);

    latchpin = 0;
    while (1) {
        latchpin = 1;
        spi.write(0);
        latchpin = 0;
    }
}
