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

#if defined(TARGET_LPC1768)
#define UART_TX             p9
#define UART_RX             p10
#define FLOW_CONTROL_RTS    p30
#define FLOW_CONTROL_CTS    p29
#define RTS_CHECK_PIN       p8
#else
#error This test is not supported on this target
#endif

Serial pc(UART_TX, UART_RX);

#ifdef RTS_CHECK_PIN
InterruptIn in(RTS_CHECK_PIN);
DigitalOut led(LED1);
static void checker(void) {
  led = !led;
}
#endif

int main() {
    char buf[256];

    pc.set_flow_control(Serial::RTSCTS, FLOW_CONTROL_RTS, FLOW_CONTROL_CTS);
#ifdef RTS_CHECK_PIN
    in.fall(checker);
#endif
    while (1) {
        pc.gets(buf, 256);
        pc.printf("%s", buf);
    }
}
