/*
 * Copyright (c) 2015-2016, ARM Limited, All Rights Reserved
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
#ifndef MBED_CORE_BUFFER_H
#define MBED_CORE_BUFFER_H

#include <stddef.h>

namespace mbed {

/** A buffer is a (pointer to data, size of data) pair
 *  It's a convenient way of passing around a pointer to a memory region
 *  and the memory size of that region in a single object
 *  It doesn't take ownership of the pointer passed to it and it doesn't do
 *  any kind of memory management.
 */
struct Buffer {
    Buffer(void *buf = NULL, size_t length = 0): buf(buf), length(length) {}

    void *buf;
    int length;
};

} // namespace mbed

#endif // #ifndef MBED_CORE_BUFFER_H

