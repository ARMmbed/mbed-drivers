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
#ifndef MBED_SPIMODULE_H
#define MBED_SPIMODULE_H

#if DEVICE_SPI

#include "platform.h"
#include "TransactionQueue.h"
#include "Transaction.h"
#include <string.h>

namespace mbed {

template<typename Class, typename Trans_type>
class SPIModule {
public:
    bool push(Transaction<Class, Trans_type>& t, int index) {
        return _queue[index].push(t);
    }

    bool pop(Transaction<Class, Trans_type>& t, int index) {
        return _queue[index].pop(t);
    }
private:
    static TransactionQueue<Transaction<Class, Trans_type>, 16> _queue[3]; // TODO number of modules from C HAL
};

template<typename Class, typename Trans_type>
TransactionQueue<Transaction<Class, Trans_type>, 16> SPIModule<Class, Trans_type>::_queue[3];

}


#endif

#endif
