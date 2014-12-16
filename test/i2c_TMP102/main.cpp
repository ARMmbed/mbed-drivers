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
#include "TMP102.h"

#if defined(TARGET_KL25Z)
TMP102 temperature(PTC9, PTC8, 0x90);

#elif defined(TARGET_LPC812)
TMP102 temperature(D10, D11, 0x90);

#elif defined(TARGET_LPC4088)
TMP102 temperature(p9, p10, 0x90);

#elif defined(TARGET_LPC2368)
TMP102 temperature(p28, p27, 0x90);

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
      defined(TARGET_NUCLEO_L152RE) || \
      defined(TARGET_LPC824)
TMP102 temperature(I2C_SDA, I2C_SCL, 0x90);

#else
TMP102 temperature(p28, p27, 0x90);
#endif

int main()
{
    float t = temperature.read();

    printf("TMP102: Temperature: %f\n\r", t);
    // In our test environment (ARM office) we should get a temperature within
    // the range ]15, 30[C
    bool result = (t > 15.0) && (t < 30.0);
    notify_completion(result);
}
