/* mbed microcontroller library
 * Copyright (c) 2015 ARM Limited.
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

#ifndef MBED_EVENTHANDLER_H
#define MBED_EVENTHANDLER_H

#include "FunctionPointer.h"
#include <cstdio>

namespace mbed {

class EventHandler {
public:
    EventHandler(void (*fptr)(void*) = NULL, void *cb_arg = NULL): _fp(fptr), _arg(cb_arg) {}

    template <typename T>
    EventHandler(T *object, void (T::*fptr)(void*), void *cb_arg = NULL): _fp(object, fptr), _arg(cb_arg) {}

    const EventHandler& arg(void *arg) {
        _arg = arg;
        return *this;
    }

    const EventHandler& arg(int arg) {
        _arg = reinterpret_cast<void*>(arg);
        return *this;
    }

    void* arg() {
        return _arg;
    }

    void call() {
        _fp.call(_arg);
    }

    void call(void *arg) {
        _fp.call(arg);
    }

#ifdef MBED_OPERATORS
    void operator ()() {
        call();
    }

    void operator ()(void *arg) {
        call(arg);
    }

    operator bool() {
        return _fp.operator bool();
    }
#endif

private:
    FunctionPointer1<void, void*> _fp;
    void *_arg;
};

} // namespace mbed

#endif // #ifndef MBED_EVENTHANDLER_H

