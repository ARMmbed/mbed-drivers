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

#if defined(TARGET_LPC1114)
DigitalOut out(dp1);
DigitalIn in(dp2);

#elif defined(TARGET_LPC1549)
// TARGET_FF_ARDUINO cannot be used, because D0 is used as USBRX (USB serial
// port pin), D1 is used as USBTX
DigitalOut out(D7);
DigitalIn in(D2);

#elif defined(TARGET_NUCLEO_F030R8) || \
      defined(TARGET_NUCLEO_F072RB) || \
      defined(TARGET_NUCLEO_F091RC) || \
      defined(TARGET_NUCLEO_F103RB) || \
      defined(TARGET_NUCLEO_F302R8) || \
      defined(TARGET_NUCLEO_F303RE) || \
      defined(TARGET_NUCLEO_F334R8) || \
      defined(TARGET_NUCLEO_F401RE) || \
      defined(TARGET_NUCLEO_F411RE) || \
      defined(TARGET_NUCLEO_L053R8) || \
      defined(TARGET_NUCLEO_L152RE)
DigitalOut out(PC_7);
DigitalIn in(PB_8);

#elif defined(TARGET_DISCO_F407VG)
DigitalOut out(PC_12);
DigitalIn in(PD_0);

#elif defined(TARGET_FF_ARDUINO)
DigitalOut out(D7);
DigitalIn in(D0);

#else
DigitalOut out(p5);
DigitalIn in(p25);

#endif

int main()
{
    out = 0;
    wait(0.1);
    if (in != 0) {
        printf("ERROR: in != 0\n");
        notify_completion(false);
    }
    out = 1;
    wait(0.1);
    if (in != 1) {
        printf("ERROR: in != 1\n");
        notify_completion(false);
    }

    notify_completion(true);
}
