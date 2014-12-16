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
#include "ADXL345.h"

#if defined(TARGET_LPC812)
ADXL345 accelerometer(D10, D11, D12, D13);

#else
ADXL345 accelerometer(p5, p6, p7, p8);
#endif

// Assume test configuration on a plane (small x and y, z ~ g)
#define MAX_X_Y     (50)
#define MIN_Z       (200)
#define MAX_Z       (300)

void check_X_Y(int v) {
    int16_t a = (int16_t)v;
    if (abs(a) > MAX_X_Y) {
        printf("X/Y acceleration is too big: %d\n", a);
        notify_completion(false);
    }
}


int main() {
    int readings[3] = {0, 0, 0};

    printf("Starting ADXL345 test...\n");
    printf("Device ID is: 0x%02x\n", accelerometer.getDevId());

    //Go into standby mode to configure the device.
    accelerometer.setPowerControl(0x00);

    //Full resolution, +/-16g, 4mg/LSB.
    accelerometer.setDataFormatControl(0x0B);

    //3.2kHz data rate.
    accelerometer.setDataRate(ADXL345_3200HZ);

    //Measurement mode.
    accelerometer.setPowerControl(0x08);

    for (int i=0; i<3; i++) {
        wait(0.1);

        //13-bit, sign extended values.
        accelerometer.getOutput(readings);

        // X and Y
        check_X_Y(readings[0]);
        check_X_Y(readings[1]);

        // Z
        int16_t z = (int16_t)readings[2];
        if ((z < MIN_Z) || (z > MAX_Z)) {
            printf("Z acceleration not within expected range\n", z);
            notify_completion(false);
        }
    }

    notify_completion(true);
}
