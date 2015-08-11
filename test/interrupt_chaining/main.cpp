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
#include "InterruptManager.h"
#include "cmsis.h"
#include "test_env.h"

Serial pc(TEST_PIN_Serial_ECHO_TX, TEST_PIN_Serial_ECHO_RX);

Ticker flipper_1;
DigitalOut led1(TEST_PIN_LED1);
int led1_state = 0;
void flip_1() {
    if (led1_state) {
        led1 = 0; led1_state = 0;
    } else {
        led1 = 1; led1_state = 1;
    }
}

class Sender {
public:
    Sender(Serial&s, char c): _s(s), _c(c) {}
    void send() { _s.putc(_c); }
private:
    Serial& _s;
    char _c;
};

Ticker flipper_2;
Sender s1(pc, '1');
Sender s2(pc, '2');

DigitalOut led2(TEST_PIN_LED2);
int led2_state = 0;
void flip_2() {
    if (led2_state) {
        led2 = 0; led2_state = 0;
    } else {
        led2 = 1; led2_state = 1;
    }
}

void testme(void) {
    pc.putc('!');
}

class Counter {
public:
    void inc(void) {
        count ++;
    }
    int get_count(void) const {
        return count;
    }
private:
    static int count;
};

int Counter::count = 0;

int main() {
    led1 = 0;
    led2 = 0;
    uint32_t initial_handler, final_handler;
    Counter c;

    // Test chaining inside Serial class
    flipper_1.attach(&flip_1, 1.0); // the address of the function to be attached (flip) and the interval (1 second)
    flipper_2.attach(&flip_2, 2.0); // the address of the function to be attached (flip) and the interval (2 seconds)

    // Test global chaining (InterruptManager)
    printf("Handler initially: %08X\n", initial_handler = NVIC_GetVector(TIMER_IRQ));
    InterruptManager *pManager = InterruptManager::get();
    pFunctionPointer_t ptm = pManager->add_handler(testme, TIMER_IRQ);
    pFunctionPointer_t pinc = pManager->add_handler_front(&c, &Counter::inc, TIMER_IRQ);
    printf("Handler after calling InterruptManager: %08X\n", NVIC_GetVector(TIMER_IRQ));

    wait(4.0);

    if (!pManager->remove_handler(ptm, TIMER_IRQ) || !pManager->remove_handler(pinc, TIMER_IRQ)) {
        printf ("remove handler failed.\n");
        notify_completion(false);
    }
    printf("Interrupt handler calls: %d\n", c.get_count());
    printf("Handler after removing previously added functions: %08X\n", final_handler = NVIC_GetVector(TIMER_IRQ));

    if (initial_handler != final_handler) {
        printf( "InteruptManager test failed.\n");
        notify_completion(false);
    }

    while(1);
}
