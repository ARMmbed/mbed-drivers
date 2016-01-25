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


DigitalOut led1(LED1);
DigitalOut led2(LED2);

Serial& computer = get_stdio_serial();

// This function is called when a character goes into the TX buffer.
void txCallback() {
    led1 = !led1;
}

// This function is called when a character goes into the RX buffer.
void rxCallback() {
    led2 = !led2;
    computer.putc(computer.getc());
}

void app_start(int, char*[]) {
    MBED_HOSTTEST_TIMEOUT(20);
    MBED_HOSTTEST_SELECT(echo);
    MBED_HOSTTEST_DESCRIPTION(serial interrupt test);
    MBED_HOSTTEST_START("MBED_14");
    computer.baud(115200);
    computer.attach(&txCallback, Serial::TxIrq);
    computer.attach(&rxCallback, Serial::RxIrq);
}
