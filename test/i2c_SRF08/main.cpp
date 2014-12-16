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
#include "SRF08.h"

DigitalOut led(LED1);

SRF08 srf08(p28, p27, 0xE0); // SDA, SCL pin and I2C address

int main() {
    printf("started\n");
    while (1) {
       printf("Measured range : %.2f cm\n",srf08.read());
       wait(1.0);
       led = !led;
    }
}
