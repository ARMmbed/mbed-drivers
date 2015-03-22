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
#include "LowPowerTimer.h"
#include "lp_ticker_api.h"

#if DEVICE_LOWPOWERTIMER

namespace mbed {

LowPowerTimer::LowPowerTimer() : _running(), _start(), _time() {
    reset();
}

void LowPowerTimer::start() {
    if (!_running) {
        _start = lp_ticker_read();
        _running = 1;
    }
}

void LowPowerTimer::stop() {
    _time += slicetime();
    _running = 0;
}

int LowPowerTimer::read_us() {
    return _time + slicetime();
}

float LowPowerTimer::read() {
    return (float)read_us() / 1000000.0f;
}

int LowPowerTimer::read_ms() {
    return read_us() / 1000;
}

int LowPowerTimer::slicetime() {
    if (_running) {
        return lp_ticker_read() - _start;
    } else {
        return 0;
    }
}

void LowPowerTimer::reset() {
    _start = lp_ticker_read();
    _time = 0;
}

#ifdef MBED_OPERATORS
LowPowerTimer::operator float() {
    return read();
}
#endif

} // namespace mbed

#endif
