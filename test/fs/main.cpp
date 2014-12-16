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
#include "rtos.h"
#include "SDFileSystem.h"

#define FILE_LOC "/sd/test.txt"

Serial pc(USBTX, USBRX);
Serial gps(p28, p27);
Serial test(p9,p10);

SDFileSystem sd(p11, p12, p13, p14, "sd");

DigitalOut myled(LED1);
DigitalOut sdled(LED2);

void sd_thread(void const *argument) {
    while (true) {
        sdled = !sdled;
        FILE *fp = NULL;
        fp = fopen(FILE_LOC, "w");
        if( fp != NULL ) fclose(fp);
        Thread::wait(1000);
    }
}

int main() {
    Thread sdTask(sd_thread, NULL, osPriorityNormal, DEFAULT_STACK_SIZE * 2.25);
    while (true) {
        myled = !myled;
        Thread::wait(1000);
    }
}
