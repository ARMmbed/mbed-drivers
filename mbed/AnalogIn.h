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
#ifndef MBED_ANALOGIN_H
#define MBED_ANALOGIN_H

#include "platform.h"
#include "analogin_api.h"

Example:
int main()
{
    uint32_t buffer[64];
    const uint32_t samples = sizeof(buffer);
    AnalogIn adc(A0);
    adc.enable();
    adc.read(buffer, samples, &onDone);
    while(!done) { sleep(); }
    adc.normalize(buffer, samples);
    // user code
    processData(buffer, samples);
    adc.disable();
}

namespace mbed {

class AnalogIn {

public:

    /** Create an AnalogIn, connected to the specified pin
     *
     *  @param pin AnalogIn pin to connect to
     */
    AnalogIn(PinName pin) {
        analogin_init(&_adc, pin);
    }

    ~AnalogIn() {
        disable();
        analogin_deinit(&_adc, pin);
    }

    void enable() {
        analogin_on(&_adc);
    }

    void disable() {
        analogin_off(&_adc);
    }

    template <typename T>
    void read_block(T *buffer, uint32_t amt, FunctionPointer0 userHandler = NULL) {
        if (NULL == userHandler) {
            analogin_read_block(&_adc, buffer, amt, sizeof(buffer), defaultHandler);
            while(!done) { sleep(); }
        } else {
            analogin_read_block(&_adc, buffer, amt, sizeof(buffer), userHandler);
        }
    }

    template <typename T>
    T read(T &value) {
        read_block(value, 1, sizeof(value));
        return value;
    }

    operator int() {
        uint32_t tmp;
        return read_block(tmp, 1, sizeof(tmp));
    }

    template <typename T>
    void normalize(T buffer, uint32_t cnt) {
        // bit twiddle and normalize a set of samples
        switch (ADC_BITS) {
            case 10:
                break;
            case 12:
                break;
            case 16:
                break;
            case 24:
                break;
            default:
                break;
        }
    }

protected:
    analogin_t _adc;
};

} // namespace mbed

#endif

#endif
