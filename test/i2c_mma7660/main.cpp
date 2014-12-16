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
#include "test_env.h"
#include "MMA7660.h"

#if defined(TARGET_FF_ARDUINO)
MMA7660 MMA(I2C_SDA, I2C_SCL);
#else
MMA7660 MMA(p28, p27);
#endif

int main() {
    if (!MMA.testConnection())
        notify_completion(false);

    for(int i = 0; i < 5; i++) {
        printf("x: %f, y: %f, z: %f\r\n", MMA.x(), MMA.y(), MMA.z());
        wait(0.2);
    }

    notify_completion(true);
}
