/* mbed Microcontroller Library
 * Copyright (c) 2015 ARM Limited
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
#ifndef MBED_LOWPOWERTIMER_H
#define MBED_LOWPOWERTIMER_H

#include "platform.h"

#if DEVICE_LOWPOWERTIMER

namespace mbed {

class LowPowerTimer {

public:
    LowPowerTimer();

    /** Start the timer
     */
    void start();

    /** Stop the timer
     */
    void stop();

    /** Reset the timer to 0.
     *
     * If it was already counting, it will continue
     */
    void reset();

    /** Get the time passed in seconds
     */
    float read();

    /** Get the time passed in mili-seconds
     */
    int read_ms();

    /** Get the time passed in micro-seconds
     */
    int read_us();

#ifdef MBED_OPERATORS
    operator float();
#endif

protected:
    int slicetime();
    int _running;          // whether the timer is running
    unsigned int _start;   // the start time of the latest slice
    int _time;             // any accumulated time from previous slices
};

} // namespace mbed

#endif

#endif
