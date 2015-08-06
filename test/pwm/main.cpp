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

int main() {

    PwmOut pwm_1(TEST_PIN_PwmOut_0);
    PwmOut pwm_2(TEST_PIN_PwmOut_1);

    pwm_1.write(0.75);
    pwm_2.write(0.50);

    printf("Initialize PwmOut_0 with duty cycle: %.2f\n", pwm_1.read());
    printf("Initialize PwmOut_1 with duty cycle: %.2f\n", pwm_2.read());

    notify_completion(true);
}
