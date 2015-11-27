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
#ifndef PINMAP_COMMON_H
#define PINMAP_COMMON_H

#include <stdint.h>
#include "PinNames.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Pin mapping structure */
typedef struct {
    PinName pin;    /**< Pin name */
    int peripheral; /**< Target specific peripheral field */
    int function;   /**< Target specific functionality field */
} PinMap;

/** Get the peripheral value in the map for the pin name
 *
 *  @param[in] pin The pin name
 *  @param[in] map The pin map
 *  @return
 *      The peripheral value for the pin name
 */
uint32_t pinmap_peripheral(PinName pin, const PinMap* map);

/** Find the function value in the map for the pin name
 *
 *  @param[in] pin The pin name
 *  @param[in] map The pin map
 *  @return
 *      The function value for the pin name
 */
uint32_t pinmap_function(PinName pin, const PinMap* map);

/** Validates two pin map values. They are valid if they are equal, or one of them is
 *  not connected. If they are not valid, error() is invoked
 *
 *  @param[in] a The pin map value a
 *  @param[in] b The pin map value b
 *  @return
 *      One of the pin map values, if the values are valid
 */
uint32_t pinmap_merge(uint32_t a, uint32_t b);

/** Set pin functionality and mode
 *
 *  @param[in] pin The pin name
 *  @param[in] map The pin map pointer
 */
void pinmap_pinout(PinName pin, const PinMap *map);

/** Get peripheral value from the pin map
 *
 *  @param[in] pin The pin name
 *  @param[in] map The pin map pointer
 *  @return
 *      The pin peripheral value
 */
uint32_t pinmap_find_peripheral(PinName pin, const PinMap* map);

/** Get peripheral function from the pin map
 *
 *  @param[in] pin The pin name
 *  @param[in] map The pin map pointer
 *  @return
 *      The pin function value
 */
uint32_t pinmap_find_function(PinName pin, const PinMap* map);

#ifdef __cplusplus
}
#endif

#endif
