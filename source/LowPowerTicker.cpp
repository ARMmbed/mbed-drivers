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
#include "LowPowerTicker.h"
#include "LowPowerTimerEvent.h"
#include "FunctionPointer.h"

#if DEVICE_LOWPOWERTIMER

namespace mbed {

void LowPowerTicker::detach() {
    remove();
    _function.attach(0);
}

void LowPowerTicker::setup(timestamp_t t) {
    remove();
    _delay = t;
    insert(_delay + lp_ticker_read());
}

void LowPowerTicker::handler() {
    insert(event.timestamp + _delay);
    _function.call();
}

} // namespace mbed

#endif
