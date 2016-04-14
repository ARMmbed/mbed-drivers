/*
 * Copyright (c) 2006-2016, ARM Limited, All Rights Reserved
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
#include "pinmap.h"
#include "mbed-drivers/mbed_error.h"

void pinmap_pinout(PinName pin, const PinMap *map) {
    if (pin == NC)
        return;

    while (map->pin != NC) {
        if (map->pin == pin) {
            pin_function(pin, map->function);

            pin_mode(pin, PullNone);
            return;
        }
        map++;
    }
    error("could not pinout");
}

uint32_t pinmap_merge(uint32_t a, uint32_t b) {
    // both are the same (inc both NC)
    if (a == b)
        return a;

    // one (or both) is not connected
    if (a == (uint32_t)NC)
        return b;
    if (b == (uint32_t)NC)
        return a;

    // mis-match error case
    error("pinmap mis-match");
    return (uint32_t)NC;
}

uint32_t pinmap_find_peripheral(PinName pin, const PinMap* map) {
    while (map->pin != NC) {
        if (map->pin == pin)
            return map->peripheral;
        map++;
    }
    return (uint32_t)NC;
}

uint32_t pinmap_peripheral(PinName pin, const PinMap* map) {
    uint32_t peripheral = (uint32_t)NC;

    if (pin == (PinName)NC)
        return (uint32_t)NC;
    peripheral = pinmap_find_peripheral(pin, map);
    if ((uint32_t)NC == peripheral) // no mapping available
        error("pinmap not found for peripheral");
    return peripheral;
}

uint32_t pinmap_find_function(PinName pin, const PinMap* map) {
    while (map->pin != NC) {
        if (map->pin == pin)
            return map->function;
        map++;
    }
    return (uint32_t)NC;
}

uint32_t pinmap_function(PinName pin, const PinMap* map) {
    uint32_t function = (uint32_t)NC;

    if (pin == (PinName)NC)
        return (uint32_t)NC;
    function = pinmap_find_function(pin, map);
    if ((uint32_t)NC == function) // no mapping available
        error("pinmap not found for function");
    return function;
}

uint32_t pinmap_peripheral_instance(uint32_t peripheral, const PinMap* map)
{
    uint32_t idx = 0;
    for (uint32_t i = 0; map[i].pin != NC; i++) {
        if ((uint32_t)map[i].peripheral == peripheral) {
            // If the peripheral is a match, it must be the first occurence
            return i ? idx + 1 : 0;
        }
        if (i) {
            // check whether this is the first occurrence of peripheral in the map
            uint32_t j;
            for (j = 0; j < i; j++) {
                if (map[i].peripheral == map[j].peripheral) {
                    break;
                }
            }
            if(i == j) {
                // this peripheral does not occur earlier in the map.
                idx++;
            }
        }
    }
    return (uint32_t)NC;
}
