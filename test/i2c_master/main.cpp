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

#define SIZE (10)
#define ADDR (0x90)

#if defined(TARGET_KL25Z)
I2C i2c(PTE0, PTE1);
#elif defined(TARGET_nRF51822)
I2C i2c(p22,p20);
#elif defined(TARGET_FF_ARDUINO)
I2C i2c(I2C_SDA, I2C_SCL);
#else
I2C i2c(p28, p27);
#endif

int main() {
    bool success = true;
    char buf[] = {3, 2, 1, 4, 5, 6, 7, 8, 9, 10};
    char res[SIZE];

    i2c.write(ADDR, buf, SIZE);
    i2c.read(ADDR, res, SIZE);

    // here should be buf[all]++
    i2c.write(ADDR, res, SIZE);
    i2c.read(ADDR, res, SIZE);

    // here should be buf[all]+=2
    i2c.write(ADDR, res, SIZE);
    i2c.write(ADDR, res, SIZE);

    // here should be buf[all]+=3
    i2c.read(ADDR, res, SIZE);
    i2c.read(ADDR, res, SIZE);

    for(int i = 0; i < SIZE; i++) {
        if (res[i] != (buf[i] + 3)) {
            success = false;
            break;
        }
    }

    notify_completion(success);
}
