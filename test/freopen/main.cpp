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
#include "TextLCD.h"

int main() {
    printf("printf to stdout\n");

    // printf to specific peripherals
    Serial pc(USBTX, USBRX);
    pc.printf("Serial(USBTX, USBRX).printf\n");

    TextLCD lcd(p14, p15, p16, p17, p18, p19, p20, "lcd"); // rs, rw, e, d0-d3, name
    lcd.printf("TextLCD.printf\n");

    // change stdout to file
    LocalFileSystem local("local");
    freopen("/local/output.txt", "w", stdout);
    printf("printf redirected to LocalFileSystem\n");
    fclose(stdout);

    // change stdout to LCD
    freopen("/lcd", "w", stdout);
    printf("printf redirected to TextLCD\n");
    fclose(stdout);

    DigitalOut led(LED1);
    while (true) {
        led = !led;
        wait(1);
    }
}
