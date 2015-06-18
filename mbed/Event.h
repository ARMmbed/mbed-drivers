/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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
#ifndef MBED_EVENT_H
#define MBED_EVENT_H

#include <string.h>
#include <stdint.h>
#include "mbed_assert.h"

namespace mbed {

class Event {
public:
    Event(void (*function)() = NULL) {
        attach_noarg(function);
    }

    Event(void (*function)(void*), void *cb_arg = NULL) {
        attach_arg(function, cb_arg);
    }

    Event(int i) {
        attach_noarg(reinterpret_cast<void(*)()>(i));
    }

    template<typename T>
    Event(T *object, void (T::*member)()) {
        attach_noarg(object, member);
    }

    template<typename T>
    Event(T *object, void (T::*member)(void*), void *arg = NULL) {
        attach_arg(object, member, arg);
    }

    void attach_noarg(void (*function)()) {
        _p._f.function_noarg = function;
        _has_arg = false;
        _membercaller_arg = 0;
    }

    void attach_arg(void (*function)(void*), void *arg) {
        _p._f.function_arg = function;
        _has_arg = true;
        _arg = arg;
        _membercaller_arg = 0;
    }

    template<typename T>
    void attach_noarg(T *object, void (T::*member)()) {
        _p.object = static_cast<void*>(object);
        _has_arg = false;
        *reinterpret_cast<void (T::**)()>(_member) = member;
        _membercaller_noarg = &Event::membercaller_noarg<T>;
    }

    template<typename T>
    void attach_arg(T *object, void (T::*member)(void*), void *arg) {
        _p.object = static_cast<void*>(object);
        _has_arg = true;
        _arg = arg;
        *reinterpret_cast<void (T::**)(void*)>(_member) = member;
        _membercaller_arg = &Event::membercaller_arg<T>;
    }

   const Event& arg(void *arg) {
        MBED_ASSERT(_has_arg);
        _arg = arg;
        return *this;
    }

    const Event& arg(int arg) {
        MBED_ASSERT(_has_arg);
        _arg = reinterpret_cast<void*>(arg);
        return *this;
    }

    void* arg() {
        MBED_ASSERT(_has_arg);
        return _arg;
    }

    void call() {
        _call(_arg);
    }

    void call(void *arg) {
        MBED_ASSERT(_has_arg);
        _call(arg);
    }

#ifdef MBED_OPERATORS
    void operator ()() {
        return call();
    }

    void operator ()(void *arg) {
        call(arg);
    }

    operator bool(void)
    {
        return (_membercaller_arg != NULL ? _p.object : (void*)_p._f.function_arg) != NULL;
    }
#endif
private:
    template<typename T>
    static void membercaller_noarg(void *object, uintptr_t *member) {
        T* o = static_cast<T*>(object);
        void (T::**m)() = reinterpret_cast<void (T::**)()>(member);
        (o->**m)();
    }

    template<typename T>
    static void membercaller_arg(void *object, uintptr_t *member, void *arg) {
        T* o = static_cast<T*>(object);
        void (T::**m)(void*) = reinterpret_cast<void (T::**)(void*)>(member);
        (o->**m)(arg);
    }

    void _call(void *arg) {
        if (_membercaller_arg == 0 && _p._f.function_arg) {
           return _has_arg ? _p._f.function_arg(_arg) : _p._f.function_noarg();
        } else if (_membercaller_arg && _p.object) {
           return _has_arg ? _membercaller_arg(_p.object, _member, arg) : _membercaller_noarg(_p.object, _member);
        }
    }

    union {
        union {
            void (*function_arg)(void*);      // function with argument
            void (*function_noarg)();         // function without argument
        } _f;
        void *object;                         // object this pointer - 0 if none attached
    } _p;
    uintptr_t _member[4];                     // aligned raw member function pointer storage - converted back by registered _membercaller
    union {
        void (*_membercaller_noarg)(void*, uintptr_t*);
        void (*_membercaller_arg)(void*, uintptr_t*, void*);
    };
    bool _has_arg;
    void *_arg;
};

typedef Event event_callback_t;

} // namespace mbed

#endif
