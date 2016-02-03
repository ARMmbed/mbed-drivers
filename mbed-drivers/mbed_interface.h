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
#ifndef MBED_INTERFACE_H
#define MBED_INTERFACE_H

#include "device.h"

/* Mbed interface mac address
 * if MBED_MAC_ADD_x are zero, interface uid sets mac address,
 * otherwise MAC_ADD_x are used.
 */
#define MBED_MAC_ADDR_INTERFACE 0x00
#define MBED_MAC_ADDR_0  MBED_MAC_ADDR_INTERFACE
#define MBED_MAC_ADDR_1  MBED_MAC_ADDR_INTERFACE
#define MBED_MAC_ADDR_2  MBED_MAC_ADDR_INTERFACE
#define MBED_MAC_ADDR_3  MBED_MAC_ADDR_INTERFACE
#define MBED_MAC_ADDR_4  MBED_MAC_ADDR_INTERFACE
#define MBED_MAC_ADDR_5  MBED_MAC_ADDR_INTERFACE
#define MBED_MAC_ADDRESS_SUM (MBED_MAC_ADDR_0 | MBED_MAC_ADDR_1 | MBED_MAC_ADDR_2 | MBED_MAC_ADDR_3 | MBED_MAC_ADDR_4 | MBED_MAC_ADDR_5)

#ifdef __cplusplus
extern "C" {
#endif

/** This returns a unique 6-byte MAC address, based on the interface UID
 * If the interface is not present, it returns a default fixed MAC address (00:02:F7:F0:00:00)
 *
 * This is a weak function that can be overwritten if you want to provide your own mechanism to
 * provide a MAC address.
 *
 *  @param mac A 6-byte array to write the MAC address
 */
void mbed_mac_address(char *mac);

/** Cause the mbed to flash the BLOD (Blue LEDs Of Death) sequence
 */
void mbed_die(void);

#ifdef __cplusplus
}
#endif

#endif
