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
#ifndef MBED_FUNCTIONPOINTER_H
#define MBED_FUNCTIONPOINTER_H

#include <string.h>
#include <stdint.h>
#include <stddef.h>

namespace mbed {

template<typename R>
class FunctionPointerBase {
public:
#ifdef MBED_OPERATORS
    operator bool(void) const {
        return _object != NULL;
    }
#endif

protected:
    /**
     * Calls the member pointed to by
     * @param arg
     * @return
     */
    R call(void* arg) {
        return _membercaller(&_object, _member, arg);
    }
    void * _object; // object Pointer/function pointer
    R (*_membercaller)(void *, uintptr_t *, void *);
    // aligned raw member function pointer storage - converted back by registered _membercaller
    uintptr_t _member[4];
};

/** A class for storing and calling a pointer to a static or member void function
 */
template <typename R>
class FunctionPointer0 : protected FunctionPointerBase<R>{
public:
    typedef R(*static_fp)(void);
    /** Create a FunctionPointer, attaching a static function
     *
     *  @param function The void static function to attach (default is none)
     */
    FunctionPointer0(static_fp function = 0) {
        attach(function);
    }

    /** Create a FunctionPointer, attaching a member function
     *
     *  @param object The object pointer to invoke the member function on (i.e. the this pointer)
     *  @param function The address of the void member function to attach
     */
    template<typename T>
    FunctionPointer0(T *object, R (T::*member)(void)) {
        attach(object, member);
    }

    /** Attach a static function
     *
     *  @param function The void static function to attach (default is none)
     */
    void attach(static_fp function) {
        FunctionPointerBase<R>::_object = reinterpret_cast<void*>(function);
        FunctionPointerBase<R>::_membercaller = &FunctionPointer0::staticcaller;
    }

    /** Attach a member function
     *
     *  @param object The object pointer to invoke the member function on (i.e. the this pointer)
     *  @param function The address of the void member function to attach
     */
    template<typename T>
    void attach(T *object, R (T::*member)(void)) {
        FunctionPointerBase<R>::_object = static_cast<void*>(object);
        *reinterpret_cast<R (T::**)(void)>(FunctionPointerBase<R>::_member) = member;
        FunctionPointerBase<R>::_membercaller = &FunctionPointer0::membercaller<T>;
    }

    /** Call the attached static or member function
     */
    R call(){
        return FunctionPointerBase<R>::call(NULL);
    }

    static_fp get_function()const {
        return reinterpret_cast<static_fp>(FunctionPointerBase<R>::_object);
    }

#ifdef MBED_OPERATORS
    R operator ()(void) {
        return call();
    }
#endif

private:
    template<typename T>
    static R membercaller(void *object, uintptr_t *member, void *arg) {
        (void) arg;
        T* o = static_cast<T*>(object);
        R (T::**m)(void) = reinterpret_cast<R (T::**)(void)>(member);
        return (o->**m)();
    }
    static R staticcaller(void *object, uintptr_t *member, void *arg) {
        (void) arg;
        (void) member;
        static_fp f = reinterpret_cast<static_fp>(object);
        return f();
    }
};

/* If we had variaditic templates, this wouldn't be a problem, but until C++11 is enabled, we are stuck with multiple classes... */

/** A class for storing and calling a pointer to a static or member void function
 */
template <typename R, typename A1>
class FunctionPointer1 : public FunctionPointerBase<R> {
protected:
    typedef struct {
        A1 a;
    } ArgStruct;
public:
    typedef R(*static_fp)(A1);
    /** Create a FunctionPointer, attaching a static function
     *
     *  @param function The void static function to attach (default is none)
     */
    FunctionPointer1(static_fp function = 0) {
        attach(function);
    }

    /** Create a FunctionPointer, attaching a member function
     *
     *  @param object The object pointer to invoke the member function on (i.e. the this pointer)
     *  @param function The address of the void member function to attach
     */
    template<typename T>
    FunctionPointer1(T *object, R (T::*member)(A1)) {
        attach(object, member);
    }

    /** Attach a static function
     *
     *  @param function The void static function to attach (default is none)
     */
    void attach(static_fp function) {
        FunctionPointerBase<R>::_object = reinterpret_cast<void*>(function);
        FunctionPointerBase<R>::_membercaller = &FunctionPointer1::staticcaller;

    }

    /** Attach a member function
     *
     *  @param object The object pointer to invoke the member function on (i.e. the this pointer)
     *  @param function The address of the void member function to attach
     */
    template<typename T>
    void attach(T *object, R (T::*member)(A1))
    {
        FunctionPointerBase<R>::_object = static_cast<void*>(object);
        *reinterpret_cast<R (T::**)(void)>(FunctionPointerBase<R>::_member) = member;
        FunctionPointerBase<R>::_membercaller = &FunctionPointer1::membercaller<T>;
    }

    /** Call the attached static or member function
     */
    R call(A1 a)
    {
        ArgStruct Args = {a};
        return FunctionPointerBase<R>::call(&Args);
    }

    static_fp get_function()const
    {
        return reinterpret_cast<static_fp>(FunctionPointerBase<R>::_object);
    }

#ifdef MBED_OPERATORS
    R operator ()(A1 a) {
        return call(a);
    }
#endif
private:
    template<typename T>
    static R membercaller(void *object, uintptr_t *member, void *arg) {
        ArgStruct *Args = static_cast<ArgStruct *>(arg);
        T* o = static_cast<T*>(object);
        R (T::**m)(A1) = reinterpret_cast<R (T::**)(A1)>(member);
        return (o->**m)(Args->a);
    }
    static R staticcaller(void *object, uintptr_t *member, void *arg) {
        ArgStruct *Args = static_cast<ArgStruct *>(arg);
        (void) member;
        static_fp f = reinterpret_cast<static_fp>(object);
        return f(Args->a);
    }
};

typedef FunctionPointer0<void> FunctionPointer;

} // namespace mbed

#endif
