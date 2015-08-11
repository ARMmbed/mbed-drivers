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

SPI spi(TEST_PIN_SPI_mosi, TEST_PIN_SPI_miso, TEST_PIN_SPI_sck);
DigitalOut cs(TEST_PIN_SPI_cs);

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
