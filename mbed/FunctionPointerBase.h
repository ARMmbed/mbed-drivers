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
#ifndef MBED_FUNCTIONPOINTERBASE_H
#define MBED_FUNCTIONPOINTERBASE_H

#include <string.h>
#include <stdint.h>
#include <stddef.h>
namespace mbed {

template<typename R>
class FunctionPointerBase {
public:
    operator bool(void) const {
        return _object != NULL;
    }

    /**
     * Calls the member pointed to by object::member or (function)object
     * @param arg
     * @return
     */
    R call(void* arg) {
        return _membercaller(_object, _member, arg);
    }

protected:
    void * _object; // object Pointer/function pointer
    R (*_membercaller)(void *, uintptr_t *, void *);
    // aligned raw member function pointer storage - converted back by registered _membercaller
    uintptr_t _member[4];
};
}
#endif
