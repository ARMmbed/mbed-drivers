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

#ifndef MBED_EVENT_H_
#define MBED_EVENT_H_

#include "FunctionPointerBind.h"
#include "FunctionPointer.h"

namespace mbed {

/** An Event binds together a function pointer and its arguments in a
 * callable object. The bound function must return 'void'. It works both
 * with functions and methods (functions inside classes).
 *
 * Sample code:
 *
 * @code
 * #include "Event.h"
 * #include <stdio.h>
 *
 * using namespace mbed;
 *
 * static void noarg_function(void) {
 *     printf("I have no arguments\r\n");
 * }
 *
 * static void int_arg_function(int arg) {
 *     printf("I have an integer argument: %d\r\n", arg);
 * }
 *
 * class FuncClass {
 * public:
 *     FuncClass(int data) : _data(data) {
 *     }
 *
 *     void str_arg_function(const char *arg) {
 *         printf("I have a string argument: %s, my data is: %d\r\n", arg, _data);
 *     }
 *
 * private:
 *     int _data;
 * };
 *
 * int main() {
 *     FuncClass test_class(10);
 *     Event noarg_event(noarg_function);
 *     Event int_arg_event1(int_arg_function, 1);
 *     Event int_arg_event2(int_arg_function, 2);
 *     Event str_arg_event(&test_class, &FuncClass::str_arg_function, "test");
 *
 *     noarg_event(); // or noarg_event.call();
 *     int_arg_event1();
 *     int_arg_event2();
 *     str_arg_event();
 *
 *     while(true);
 * }
 * @endcode
 *
 * Output of above sequence is:
 *
 * @code
 * I have no arguments
 * I have an integer argument: 1
 * I have an integer argument: 2
 * I have a string argument: test, my data is: 10
 * @endcode
 *
 * The bind function can also be used (outside of the Event class) to bind a function
 * and its arguments (where applicable) into an Event.
 */

class Event: public FunctionPointerBind<void> {
/** An event binds a function pointer with its argument(s) in a callable object.
 *  The function pointer must have 'void' as the return type.
 */
public:
    /** Creates an empty (unbount) Event
     */
    Event(): FunctionPointerBind<void>() {
    }

    /** Forward FunctionPointerBind arguments to the base class
     */
    Event(const FunctionPointerBind<void>& fp): FunctionPointerBind<void>(fp) {
    }

    /**************************************************************************/
    // Functions with no arguments

    /** Creates an event, binding it to a function without arguments
     *  @param pfn pointer to the function to bind
     */
    Event(void (*pfn)()): FunctionPointerBind<void>(FunctionPointer0<void>(pfn).bind()) {
    }

    /** Create an event, binding it to a method without arguments
     *  @param object the method's instance
     *  @param member pointer to the method to bind
     */
    template <typename T>
    Event(T *object, void (T::*member)()): FunctionPointerBind<void>(FunctionPointer0<void>(object, member).bind()) {
    }

    /** Creates an event by binding an existent FunctionPointer0 instance
     *  @param fp the FunctionPointer0 instance
     */
    Event(FunctionPointer0<void>& fp): FunctionPointerBind<void>(fp.bind()) {
    }

    /**************************************************************************/
    // Functions with one argument

    /** Creates an event, binding it to a function that receives an argument and a specific argument
     *  @param pfn pointer to the function to bind
     *  @param arg argument to bind
     */
    template<typename A1>
    Event(void (*pfn)(A1), A1 arg): FunctionPointerBind<void>(FunctionPointer1<void, A1>(pfn).bind(arg)) {
    }

    /** Creates an event, binding it to a method that receives one argument and a specific argument
     *  @param object the method's instance
     *  @param member ponter to the method to bind
     *  @param arg argument to bind
     */
    template <typename T, typename A1>
    Event(T *object, void (T::*member)(A1), A1 arg): FunctionPointerBind<void>(FunctionPointer1<void, A1>(object, member).bind(arg)) {
    }

    /** Creates an event by binding an existent FunctionPointer1 instance
     *  @param fp the FunctionPointer1 instance
     *  @param arg argument to bind
     */
    template <typename A1>
    Event(FunctionPointer1<void, A1>& fp, A1 arg): FunctionPointerBind<void>(fp.bind(arg)) {
    }
};

/** Binds a function without arguments into an Event
 *  @param  pfn pointer to the function to bind
 *  @return the Event instance
 */
Event bind(void (*pfn)()) {
    return Event(pfn);
}

/** Binds a method without arguments into an Event
 *  @param  object the method's instance
 *  @param  member pointer to the method to bind
 *  @return the Event instance
 */
template <typename T>
Event bind(T *object, void (T::*member)()) {
    return Event(object, member);
}

/** Binds a FunctionPointer0 into an Event
 *  @param  fp the FunctionPointer0 instance
 *  @return the Event instance
 */
template <typename T>
Event bind(FunctionPointer0<void>& fp) {
    return Event(fp);
}

/** Binds a function with an agrument and a specific argument into an Event
 *  @param  pfn pointer to the function to bind
 *  @param  arg argument to bind
 *  @return the Event instance
 */
template<typename A1>
Event bind(void (*pfn)(A1), A1 arg) {
    return Event(pfn, arg);
}

/** Binds a method with an argument and a specific argument into an Event
 *  @param  object the method's instance
 *  @param  membter pointer to the method to bind
 *  @param  arg argument to bind
 *  @return the Event instanve
 */
template <typename T, typename A1>
Event bind(T *object, void (T::*member)(A1), A1 arg) {
    return Event(object, member, arg);
}

/** Binds a FunctionPointer1 and a specific argument into an Event
 *  @param  fp the FunctionPointer1 instance
 *  @parm   arg argument to bind
 *  @return the Event instanve
 */
template <typename A1>
Event bind(FunctionPointer1<void, A1>& fp, A1 arg) {
    return Event(fp, arg);
}

} // namespace mbed

#endif // MBED_EVENT_H_
