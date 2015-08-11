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

/******************************************************************************
*  This will test an I2C EEPROM connected to mbed by writing a predefined byte at
*  address 0 and then reading it back and comparing it with the known byte value a
*  number of times. This test was written specifically for reproducing the bug
*  reported here:
*
*  https://mbed.org/forum/bugs-suggestions/topic/4128/
*
*  Test configuration:
*
* set 'ntests' to the number of iterations
* set 'i2c_speed_hz' to the desired speed of the I2C interface
* set 'i2c_delay_us' to the delay that will be inserted between 'write' and
*  'read' I2C operations (https://mbed.org/users/mbed_official/code/mbed/issues/1
*  for more details). '0' disables the delay.
* define I2C_EEPROM_VERBOSE to get verbose output
*
*  The test ran with a 24LC256 external EEPROM memory, but any I2C EEPROM memory
*  that uses two byte addresses should work.
******************************************************************************/

// Test configuration block
namespace
{
const int ntests = 1000;
const int i2c_freq_hz = 400000;
const int i2c_delay_us = 0;
// const int EEPROM_24LC256_SIZE = (256 * 1024 / 8);   // 256 kbit memory
}
// End of test configuration block

I2C i2c(TEST_PIN_I2C_SDA, TEST_PIN_I2C_SCL);

#define PATTERN_MASK 0x66, ~0x66, 0x00, 0xFF, 0xA5, 0x5A, 0xF0, 0x0F

int main()
{
    const int EEPROM_MEM_ADDR = TEST_EEPROM_MEM_ADDR;
    bool result = true;

    i2c.frequency(i2c_freq_hz);
    printf("I2C: I2C Frequency: %d Hz\r\n", i2c_freq_hz);

    printf("I2C: Lines pattern write test ... ");
    int write_errors = 0;
    for (int i = 0; i < ntests; i++) {
        char data[] = { 0 /*MSB*/, 0 /*LSB*/, PATTERN_MASK };
        const int addr = i * 8;   // 8 bytes of data in data array
        data[0] = ((0xFF00 & addr) >> 8) & 0x00FF;
        data[1] = (addr & 0x00FF);

        if (i2c.write(EEPROM_MEM_ADDR, data, sizeof(data)) != 0) {
            write_errors++;
        }

        while (i2c.write(EEPROM_MEM_ADDR, NULL, 0)) ; // wait to complete

        // us delay if specified
        if (i2c_delay_us != 0) {
            wait_us(i2c_delay_us);
        }
    }

    printf("[%s]\r\n", write_errors ? "FAIL" : "OK");
    printf("I2C: Write errors: %d ... [%s]\r\n", write_errors, write_errors ? "FAIL" : "OK");

    printf("I2C: Lines pattern read test ... ");
    int read_errors = 0;
    int pattern_errors = 0;
    for (int i = 0; i < ntests; i++) {
        char data[8] = { 0 };   // General puspose buffer
        const int addr = i * 8; // 8 bytes of data in data array
        data[0] = ((0xFF00 & addr) >> 8) & 0x00FF;
        data[1] = (addr & 0x00FF);

        // Set address for read
        if (i2c.write(EEPROM_MEM_ADDR, data, 2, true) != 0) {
        }

        if (i2c.read(EEPROM_MEM_ADDR, data, 8) != 0) {
            read_errors++;
        }

        static char pattern[] = { PATTERN_MASK };
        if (memcmp(pattern, data, sizeof(data))) {
            pattern_errors++;
        }
    }

    printf("[%s]\r\n", read_errors ? "FAIL" : "OK");
    printf("I2C: Read errors: %d/%d ... [%s]\r\n", read_errors, ntests, read_errors ? "FAIL" : "OK");
    printf("EEPROM: Pattern match errors: %d/%d ... [%s]\r\n", pattern_errors, ntests, pattern_errors ? "FAIL" : "OK");

    result = write_errors == 0 && read_errors == 0;
    notify_completion(result);
}
