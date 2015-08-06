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

InterruptIn button(TEST_PIN_InterruptIn_0);
InterruptIn button1(TEST_PIN_InterruptIn_1);
InterruptIn button2(TEST_PIN_InterruptIn_2);
InterruptIn button3(TEST_PIN_InterruptIn_3);
InterruptIn button4(TEST_PIN_InterruptIn_4);
InterruptIn button5(TEST_PIN_InterruptIn_5);
InterruptIn button6(TEST_PIN_InterruptIn_6);
InterruptIn button7(TEST_PIN_InterruptIn_7);
InterruptIn button8(TEST_PIN_InterruptIn_8);
InterruptIn button9(TEST_PIN_InterruptIn_9);
DigitalOut led(TEST_PIN_LED1);
DigitalOut flash(TEST_PIN_LED2);

void flip() {
    led = !led;
}

int main() {
    flash = 0;
    led = 0;
    button.mode(PullUp);
    button.rise(&flip);  // attach the address of the flip function to the rising edge
    button1.rise(&flip);
    button2.rise(&flip);
    button3.rise(&flip);
    button4.rise(&flip);
    button5.rise(&flip);
    button6.rise(&flip);
    button7.rise(&flip);
    button8.rise(&flip);
    button9.rise(&flip);

    while(1) {           // wait around, interrupts will interrupt this!
        flash = !flash;
        wait(0.25);
    }
}
