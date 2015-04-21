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
#include "CircularBuffer.h"
#include "Transaction.h"

namespace mbed {

/** SPI module which contains transaction queue for spi peripheral.
 */
template<typename Class>
class SPIModule {
public:
   /** Push SPI transaction
     *
     * @return True if the transaction was added, false otherwise
     */
    bool push(Transaction<Class>& t, int index) {
        return _queue[index].push(t);
    }

   /** Pop SPI transaction
     *
     * @return True if the transaction was popped (the queue was not empty), false otherwise
     */
    bool pop(Transaction<Class>& t, int index) {
        return _queue[index].pop(t);
    }
private:
    static CircularBuffer<Transaction<Class>, TRANSACTION_QUEUE_SIZE_SPI> _queue[MODULES_SIZE_SPI];
};

template<typename Class>
CircularBuffer<Transaction<Class>, TRANSACTION_QUEUE_SIZE_SPI> SPIModule<Class>::_queue[MODULES_SIZE_SPI];

}

#endif

#endif
