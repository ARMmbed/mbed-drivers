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

Serial pc(USBTX, USBRX);

extern "C" void mbed_reset();

int main() {
    pc.printf("start\n");
    wait(1);

    unsigned int counter = 0;
    while(1) {
        pc.printf("%u\n",counter++);
        wait(1);
        mbed_reset();
    }
}
