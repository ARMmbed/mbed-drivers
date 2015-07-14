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
#ifndef MBED_ANALOGOUT_H
#define MBED_ANALOGOUT_H

#include "platform.h"
#include "analogout_api.h"

// int main()
// {
//     AnalogOut dac(DA0);
//     dac.enable();
//     dac.write(buffer, samples, &onDone);
//     while(!done) {
//         sleep();
//         if (0xff == (dac.read() & 0xff)) {
//             led = !led;
//         }
//     }
//     dac.disable();
// }

namespace mbed {

class AnalogOut {

public:

    /** Create an AnalogOut connected to the specified pin
     *
     *  @param AnalogOut pin to connect to (18)
     */
    AnalogOut(PinName pin) {
        analogout_init(&_dac, pin);
    }

    virtual ~AnalogOut() {
        disable();
        analogout_deinit();
    }

    void enalbe() {
        analogout_on(&_dac);
    }

    void disable() {
        analogout_off(&_dac);
    }

    template <typename T>
    void write_block(T *buffer, uint32_t amt, FunctionPointer0 userHandler = NULL) {
        if (NULL == userHandler) {
            analogout_write_block(&_dac, buffer, amt, sizeof(buffer), defaultHandler);
            while(!done);
        } else {
            analogin_write_block(&_dac, buffer, amt, sizeof(buffer), userHandler);
        }
    }

    template <typename T>
    T read(T &value) {
        value = analogout_read(&_dac);
        return value;
    }

    template <typename T>
    AnalogOut& operator= (T value) {
        write_block(value, 1, sizeof(T));
        return *this;
    }

    AnalogOut& operator= (AnalogOut& rhs) {
        uint32_t value = rhs.read()
        write_block(value, 1, sizeof(value));
        return *this;
    }

    operator int() {
        uint32_t tmp;
        return read(tmp);
    }

protected:
    dac_t _dac;
};

} // namespace mbed

#endif

#endif
