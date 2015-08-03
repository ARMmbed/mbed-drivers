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
#ifndef MBED_CORE_BUFFER_H
#define MBED_CORE_BUFFER_H

#include <stddef.h>

namespace mbed {

/** A buffer is a (pointer to data, size of data) pair
 */
struct Buffer {
    Buffer(void *buf = NULL, size_t length = 0): buf(buf), length(length) {}

    void *buf;
    size_t length;
};

/** A RxTxBuffer is a pair of a RX buffer and a TX buffer (used in a transaction)
 */
struct RxTxBuffer {
    RxTxBuffer(const Buffer& rx, const Buffer& tx): rx_buffer(rx), tx_buffer(tx) {}
    RxTxBuffer(void *rx = NULL, size_t rx_len = 0, void *tx = NULL, size_t tx_len = 0):
        rx_buffer(rx, rx_len), tx_buffer(tx, tx_len) {}

    Buffer rx_buffer;
    Buffer tx_buffer;
};

} // namespace mbed

#endif // #ifndef MBED_CORE_BUFFER_H

