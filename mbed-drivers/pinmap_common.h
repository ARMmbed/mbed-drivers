/*
 * Copyright (c) 2015-2016, ARM Limited, All Rights Reserved
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
#ifndef PINMAP_COMMON_H
#define PINMAP_COMMON_H

#include <stdint.h>
#include "PinNames.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    PinName pin;
    int peripheral;
    int function;
} PinMap;

uint32_t pinmap_peripheral(PinName pin, const PinMap* map);
uint32_t pinmap_function(PinName pin, const PinMap* map);
uint32_t pinmap_merge(uint32_t a, uint32_t b);
void     pinmap_pinout(PinName pin, const PinMap *map);
uint32_t pinmap_find_peripheral(PinName pin, const PinMap* map);
uint32_t pinmap_find_function(PinName pin, const PinMap* map);
/**
 * @brief A function for obtaining a unique index for a given peripheral
 *
 * This function provides a mechanism to convert an implementation-defined peripheral ID into a peripheral index. The
 * index is obtained by order of first occurrence in the map provided. In order to prevent clashes in multi-pin
 * peripherals, the convention is to always use the data transmit pin's map:
 *
 * * I2C: PinMap_I2C_SDA
 * * Serial: PinMap_UART_TX
 * * SPI: PinMap_SPI_MOSI
 * * ADC: PinMap_ADC
 * * DAC: PinMap_DAC
 * * PWM: PinMap_PWM
 *
 * Note that the peripheral index is not defined to match any kind of device-defined ordering; instead, it guarantees a
 * unique index for every logical peripheral.
 *
 * @param[in] peripheral The peripheral ID to search for in map
 */
uint32_t pinmap_peripheral_instance(uint32_t peripheral, const PinMap* map);

#ifdef __cplusplus
}
#endif

#endif
