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
#include "LowPowerTimerEvent.h"
#include "cmsis.h"

#include <stddef.h>

#if DEVICE_LOWPOWERTIMER

namespace mbed {

LowPowerTimerEvent::LowPowerTimerEvent() : event() {
    lp_ticker_set_handler((&LowPowerTimerEvent::irq));
}

void LowPowerTimerEvent::irq(uint32_t id) {
    LowPowerTimerEvent *timer_event = (LowPowerTimerEvent*)id;
    timer_event->handler();
}

LowPowerTimerEvent::~LowPowerTimerEvent() {
    remove();
}

// insert in to linked list
void LowPowerTimerEvent::insert(timestamp_t timestamp) {
    lp_ticker_insert_event(&event, timestamp, (uint32_t)this);
}

void LowPowerTimerEvent::remove() {
    lp_ticker_remove_event(&event);
}

} // namespace mbed

#endif
