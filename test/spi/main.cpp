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

SPI spi(TEST_PIN_SPI_mosi, TEST_PIN_SPI_miso, TEST_PIN_SPI_sck);
DigitalOut latchpin(TEST_PIN_SPI_cs);


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
