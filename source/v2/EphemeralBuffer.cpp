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
#include "mbed-drivers/v2/EphemeralBuffer.hpp"
#include <cstring>
namespace mbed {
namespace drivers {
namespace v2 {

void EphemeralBuffer::set(const Buffer & b)
{
    set(b.buf,b.length);
}

void EphemeralBuffer::set(void *buf, size_t len)
{
    _ephemeral = false;
    _ptrLen = len;
    _dataPtr = buf;
}

void EphemeralBuffer::set_ephemeral(const Buffer & b)
{
    set_ephemeral(b.buf,b.length);
}

void EphemeralBuffer::set_ephemeral(void *buf, size_t len)
{
    if (len <= sizeof(_data)) {
        _ephemeral = true;
        _len = len;
        if (buf) {
            std::memcpy(_data, buf, len);
        }
    } else {
        _ephemeral = false;
        _ptrLen = len;
        _dataPtr = buf;
    }
}

void * EphemeralBuffer::get_buf()
{
    if (_ephemeral) {
        return _data;
    } else {
        return _dataPtr;
    }
}

size_t EphemeralBuffer::get_len() const
{
    if (_ephemeral) {
        return _len;
    } else {
        return _ptrLen;
    }
}

bool EphemeralBuffer::is_ephemeral() const
{
    return _ephemeral;
}
} // namespace v2
} // namespace drivers
} // namespace mbed
