/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
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

#ifndef MBED_FUNCTIONPOINTERBIND_H__
#define MBED_FUNCTIONPOINTERBIND_H__

#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include "FunctionPointerBase.h"

#ifndef EVENT_STORAGE_SIZE
#define EVENT_STORAGE_SIZE 32
#endif

namespace mbed{

template<typename R>
class FunctionPointerBind {
	friend class FunctionPointerBase<R>;
public:
    // Call the Event
    void call() {
        _fp.call(reinterpret_cast<void *>(_storage));
    }

    void bind_manual(void * args, size_t argsize) {
    	assert(argsize <= sizeof(_storage));
    	memcpy(_storage, args, argsize);
    }

    // attach a function and its arguments
    void attach(FunctionPointerBase<R> &fp) {
        _fp = fp;
    }

private:
    FunctionPointerBase<R> _fp;
    uint32_t _storage[(EVENT_STORAGE_SIZE+sizeof(uint32_t)-1)/sizeof(uint32_t)];
};
typedef FunctionPointerBind<void> Event;
}

#endif // MBED_FUNCTIONPOINTERBIND_H__
