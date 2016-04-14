/*
 * Copyright (c) 2006-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "mbed-drivers/Ticker.h"

#include "mbed-drivers/TimerEvent.h"
#include "core-util/FunctionPointer.h"
#include "ticker_api.h"

namespace mbed {

void Ticker::detach() {
    remove();
    _function.attach(0);
}

void Ticker::setup(timestamp_t t) {
    remove();
    _delay = t;
    insert(_delay + ticker_read(_ticker_data));
}

void Ticker::handler() {
    insert(event.timestamp + _delay);
    _function.call();
}

} // namespace mbed
