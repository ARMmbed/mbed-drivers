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

#ifndef __MBED_CORE_MBED_APP_H__
#define __MBED_CORE_MBED_APP_H__

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/** app_start is used to set up the user's application. It should not block.
 *
 * Use this function to schedule the first code that your app will execute, and
 * perform non-blocking initialisation.
 *
 * @code
 * using namespace mbed::minar;
 * void app_start(int argc, char *argv[]){
 *     Scheduler::postCallback(&doSomething);
 *     Scheduler::postCallback(&doSomethingElse).delay(milliseconds(100)).period(milliseconds(200));
 * }
 * @endcode
 */

void app_start(int argc, char *argv[]);

#ifdef __cplusplus
} // extern "C"
#endif // #ifdef __cplusplus

#endif // ndef __MBED_CORE_MBED_APP_H__

