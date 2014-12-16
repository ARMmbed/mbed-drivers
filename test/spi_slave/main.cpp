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

#if defined(TARGET_KL25Z)
SPISlave device(PTD2, PTD3, PTD1, PTD0);    // mosi, miso, sclk, ssel
#elif defined(TARGET_nRF51822)
SPISlave device(p12, p13, p15, p14);  // mosi, miso, sclk, ssel
#elif defined(TARGET_LPC812)
SPISlave device(P0_14, P0_15, P0_12, P0_13);    // mosi, miso, sclk, ssel
#elif defined(TARGET_FF_ARDUINO)
SPISlave device(D11, D12, D13, D10);       // mosi, miso, sclk, ssel
#else
SPISlave device(p5, p6, p7, p8);            // mosi, miso, sclk, ssel
#endif


int main() {
    uint8_t resp = 0;

    device.reply(resp);                    // Prime SPI with first reply

    while(1) {
        if(device.receive()) {
            resp = device.read();           // Read byte from master and add 1
            device.reply(resp);             // Make this the next reply
        }
    }
}
