/* mbed Microcontroller Library
 * Copyright (c) 2015-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
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
#ifndef MBED_DRIVERS_EPHEMERAL_BUFFER_H
#define MBED_DRIVERS_EPHEMERAL_BUFFER_H

#include <cstdint>
#include <cstring>
#include "mbed-drivers/Buffer.h"

namespace mbed {
namespace drivers {
namespace v2 {
/**
 * The EphemeralBuffer class is a variant of the Buffer class.
 * Instead of just storing a buffer pointer and a size, if the buffer is less than ```sizeof(size_t) + sizeof(void *)```
 * bytes long, it packs the whole buffer into the space occupied by the pointer and size variable. This is indicated by
 * setting the MSB in size.
 *
 * EphemeralBuffer is not assumed to own the buffer to which it points unless it
 * is in ephemeral mode.
 */
class EphemeralBuffer {
public:
    /**
     * @brief Default constructor
     * Initializes the EphemeralBuffer to ephemeral mode with a buffer length of
     * 7. This means that a default EphemeralBuffer can be used to receive data
     * or as the target of a copy operation.
     */
    EphemeralBuffer() : _len(sizeof(_data)), _ephemeral(true) {}

    /**
     * @brief Copy constructor
     * Duplicates the incoming EphemeralBuffer.
     *
     * @param[in] x the buffer to copy
     */
    EphemeralBuffer(const EphemeralBuffer & x) : _ephemeral(x._ephemeral)
    {
        if (is_ephemeral()) {
            _len = x._len;
            std::memcpy(_data, x._data, sizeof(_data));
        } else {
            _dataPtr = const_cast<void *>(x._dataPtr);
            _ptrLen  = x._ptrLen;
        }
    }

    /**
     * Set buffer pointer and length.
     *
     * @param[in] b A buffer to duplicate
     */
    void set(const Buffer & b);

    /**
     * Set buffer pointer and length.
     * If the buffer is 7 or fewer bytes, copy it into the contents of EphemeralBuffer
     * instead of keeping a pointer to it.
     *
     * @param[in] b A buffer to duplicate
     */
    void set_ephemeral(const Buffer & b);

    /**
     * Set the buffer pointer and length
     *
     * @param[in] buf the buffer pointer to duplicate
     * @param[in] len the length of the buffer to duplicate
     */
    void set(void *buf, std::size_t len);

    /**
     * Set buffer pointer and length.
     * If the buffer is 7 or fewer bytes, copy it into the contents of EphemeralBuffer
     * instead of keeping a pointer to it.
     *
     * @param[in] buf the buffer pointer to duplicate
     * @param[in] len the length of the buffer to duplicate
     */
    void set_ephemeral(void *buf, std::size_t len);

    /**
     * Get a pointer to the buffer.
     *
     * If the buffer is the internal storage, return it, otherwise return the reference
     *
     * @return the buffer pointer
     */
    void * get_buf();

    /**
     * Get the length
     *
     * @return the length of the buffer
     */
    std::size_t get_len() const;

    /**
     * Check if the buffer is ephemeral.
     *
     * @retval true The buffer contains data, rather than a pointer
     * @retval false The buffer contains a pointer to data
     */
    bool is_ephemeral() const;

protected:
    union {
        struct {
            void * _dataPtr;
            std::size_t _ptrLen:(sizeof(size_t)*8-1);
            unsigned _reserved:1;
        };
        struct {
            std::uint8_t _data[sizeof(void *) + sizeof(size_t) - 1];
            std::size_t _len:7;
            unsigned _ephemeral:1;
        };
    };
public:
    /**
     * A constant that indicates the maximum size of the buffer in ephemeral mode.
     */
    static constexpr size_t ephemeralSize = sizeof(_data);

};
} // namespace v2
} // namespace drivers
} // namespace mbed
#endif // MBED_DRIVERS_EPHEMERAL_BUFFER_H
