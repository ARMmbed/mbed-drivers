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

// this header is a no-op if included from C: the app_start function must be
// implemented in C++
#ifdef __cplusplus

#ifndef __MBED_CORE_MBED_APP_H__
#define __MBED_CORE_MBED_APP_H__

/** app_start is used to set up the user's application. It should not block.
 *
 * Use this function to schedule the first code that your app will execute, and
 * perform non-blocking initialisation.
 *
 * @code
 * using namespace mbed::minar;
 * void app_start(Scheduler* sched){
 *     sched->postCallback(&doSomething);
 *     sched->postCallback(&doSomethingElse).delay(milliseconds(100)).period(milliseconds(200));
 * }
 * @endcode
 */

// forward-declare the Scheduler instance type:
namespace minar{
class Scheduler;
} // namespace minar

void app_start(minar::Scheduler* sched);

#endif // ndef __MBED_CORE_MBED_APP_H__

#endif // #ifdef __cplusplus

