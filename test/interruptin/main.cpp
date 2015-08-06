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

DigitalOut myled(TEST_PIN_LED1);
DigitalOut led2(TEST_PIN_LED2);

volatile int checks = 0;
void in_handler() {
    checks++;
    led2 = !led2;
}

DigitalOut out(TEST_PIN_DigitalOut);
InterruptIn in(TEST_PIN_InterruptIn);

#define IN_OUT_SET      out = 1; myled = 1;
#define IN_OUT_CLEAR    out = 0; myled = 0;

void flipper() {
    for (int i = 0; i < 5; i++) {
        IN_OUT_SET;
        wait(0.2);
        IN_OUT_CLEAR;
        wait(0.2);
    }
}

int main() {
    IN_OUT_CLEAR;
    //Test falling edges first
    in.rise(NULL);
    in.fall(in_handler);
    flipper();

    if(checks != 5) {
        printf("MBED: falling edges test failed: %d\r\n",checks);
        notify_completion(false);
    }

    //Now test rising edges
    in.rise(in_handler);
    in.fall(NULL);
    flipper();

    if (checks != 10) {
        printf("MBED: raising edges test failed: %d\r\n", checks);
        notify_completion(false);
    }

    //Now test switch off edge detection
    in.rise(NULL);
    in.fall(NULL);
    flipper();

    if (checks != 10) {
        printf("MBED: edge detection switch off test failed: %d\r\n", checks);
        notify_completion(false);
    }

    //Finally test both
    in.rise(in_handler);
    in.fall(in_handler);
    flipper();

    if (checks != 20) {
        printf("MBED: Simultaneous rising and falling edges failed: %d\r\n", checks);
        notify_completion(false);
    }

    notify_completion(true);
    return 0;
}
