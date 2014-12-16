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

#if defined(TARGET_LPC4088)
InterruptIn button(p18);
InterruptIn button1(p17);
InterruptIn button2(p16);
InterruptIn button3(p15);
InterruptIn button4(p14);
InterruptIn button5(p13);
InterruptIn button6(p12);
InterruptIn button7(p11);
InterruptIn button8(p10);
InterruptIn button9(p9);
DigitalOut led(LED1);
DigitalOut flash(LED4);

#elif defined(TARGET_LPC1114)
InterruptIn button(p30); // SW2 (User switch)
InterruptIn button1(p5);
InterruptIn button2(p6);
InterruptIn button3(p7);
InterruptIn button4(p9);
InterruptIn button5(p10);
InterruptIn button6(p12);
InterruptIn button7(p13);
InterruptIn button8(p14);
InterruptIn button9(p15);
DigitalOut led(LED1);
DigitalOut flash(LED2);

#else
InterruptIn button(p30);
InterruptIn button1(p29);
InterruptIn button2(p28);
InterruptIn button3(p27);
InterruptIn button4(p26);
InterruptIn button5(p25);
InterruptIn button6(p24);
InterruptIn button7(p23);
InterruptIn button8(p22);
InterruptIn button9(p21);
DigitalOut led(LED1);
DigitalOut flash(LED4);
#endif

void flip() {
    led = !led;
}

int main() {
    flash = 0;
    led = 0;
#if defined(TARGET_LPC1114)
    button.mode(PullUp);
#endif
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
