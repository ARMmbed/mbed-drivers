/* mbed Microcontroller Library
 * Copyright (c) 2013-2014 ARM Limited
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
#include "test_env.h"
#include "semihost_api.h"

#define MAC_VENDOR_ARM_0    0x00
#define MAC_VENDOR_ARM_1    0x02
#define MAC_VENDOR_ARM_2    0xF7

int main() {

    printf("Semihost connected: %s\n", (semihost_connected()) ? ("Yes") : ("No"));

    char uid[DEVICE_ID_LENGTH + 1] = {0};
    bool result = true;

    const int ret = mbed_interface_uid(uid);
    if (ret == 0) {
        printf("UID: %s\r\n", uid);
    }
    else {
        result = false;
    }

    char mac[6] = {0};  // @param mac A 6-byte array to write the MAC address
    mbed_mac_address(mac);
    printf("MAC Address: %02X:%02X:%02X:%02X:%02X:%02X\r\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    if (mac[0] == MAC_VENDOR_ARM_0 &&
        mac[1] == MAC_VENDOR_ARM_1 &&
        mac[2] == MAC_VENDOR_ARM_2) {
        printf("MAC Address Prefix: 00:02:F7, Vendor: ARM\r\n");
    }

    notify_completion(result);
    return 0;
}
