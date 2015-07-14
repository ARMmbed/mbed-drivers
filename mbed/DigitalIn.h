/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
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
#ifndef MBED_DIGITALIN_H
#define MBED_DIGITALIN_H

#include "platform.h"
#include "gpio_api.h"

namespace mbed {

class DigitalIn {

public:
    /** Create a DigitalIn connected to the specified pin
     *
     *  @param pin DigitalIn pin to connect to
     */
    DigitalIn(PinName pin) : _gpio() {
        gpio_init_in(&_gpio, pin);
    }

    /** Create a DigitalIn connected to the specified pin
     *
     *  @param pin DigitalIn pin to connect to
     *  @param mode the initial mode of the pin
     */
    DigitalIn(PinName pin, PinMode mode) : _gpio() {
        gpio_init_in_ex(&_gpio, pin, mode);
    }

    ~DigitalIn() {
        gpio_deinit(&_gpio);
    }

    /** Read the input, represented as 0 or 1 (int)
     *
     *  @returns
     *    An integer representing the state of the input pin,
     *    0 for logical 0, 1 for logical 1
     */
    int read() {
        return gpio_read(&_gpio);
    }

    /** Set the input pin mode
     *
     *  @param mode PullUp, PullDown, PullNone, OpenDrain, PullDefault
     */
    void mode(PinMode pull) {
        gpio_mode(&_gpio, pull);
    }

    /** An operator shorthand for read()
     */
    operator int() {
        return read();
    }

protected:
    gpio_t _gpio;
};

} // namespace mbed

#endif
