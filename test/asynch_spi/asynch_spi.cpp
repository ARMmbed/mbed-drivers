
/* mbed Microcontroller Library
 * Copyright (c) 2013 ARM Limited
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
#include "mbed.h"
#include <stdio.h>
#include "minar/minar.h"
#include "mbed-util/Event.h"

#if DEVICE_SPI_ASYNCH

#define SHORT_XFR 3
#define LONG_XFR 16
#define TEST_BYTE0 0x00
#define TEST_BYTE1 0x11
#define TEST_BYTE2 0xFF
#define TEST_BYTE3 0xAA
#define TEST_BYTE4 0x55
#define TEST_BYTE5 0x50

#define TEST_BYTE_RX TEST_BYTE3
#define TEST_BYTE_TX_BASE TEST_BYTE5

#if defined(TARGET_K64F)
#define TEST_MOSI_PIN PTD2
#define TEST_MISO_PIN PTD3
#define TEST_SCLK_PIN PTD1
#define TEST_CS_PIN   PTD0
#else
#error Target not supported
#endif

using namespace minar;

class SPITest {

public:
    SPITest(): spi(TEST_MOSI_PIN, TEST_MISO_PIN, TEST_SCLK_PIN), cs(TEST_CS_PIN) {
        for (uint32_t i = 0; i < sizeof(tx_buf); i++) {
            tx_buf[i] = i + TEST_BYTE_TX_BASE;
        }
        cs = 1;
    }

    void start() {
        printf("Starting short transfer test\r\n");
        init_rx_buffer();
        cs = 0;
        printf("Res is %d\r\n", spi.transfer(tx_buf, SHORT_XFR, rx_buf, SHORT_XFR, SPI::event_callback_t(this, &SPITest::short_transfer_complete_cb), SPI_EVENT_COMPLETE));
    }

private:
    void init_rx_buffer() {
        for (uint32_t i = 0; i < sizeof(rx_buf); i ++) {
            rx_buf[i] = 0;
        }
    }

    void compare_buffers(const uint8_t *src, const uint8_t *dest, uint32_t len) {
         for (uint32_t i = 0; i < len; i ++) {
            if (src[i] != dest[i]) {
                printf("MISMATCH at position %u: expected %d, got %d\r\n", (unsigned)i, (int)src[i], (int)dest[i]);
            }
        }
    }

    void short_transfer_complete_cb(Buffer tx_buffer, Buffer rx_buffer, int event) {
        cs = 1;
        printf("Short transfer DONE, event is %d\r\n", event);
        compare_buffers((uint8_t*)rx_buffer.buf, (uint8_t*)tx_buffer.buf, rx_buffer.length);
        printf("Starting long transfer test\r\n");
        init_rx_buffer();
        cs = 0;
        printf("Res is %d\r\n", spi.transfer(tx_buf, LONG_XFR, rx_buf, LONG_XFR, SPI::event_callback_t(this, &SPITest::long_transfer_complete_cb), SPI_EVENT_COMPLETE));
    }

    void long_transfer_complete_cb(Buffer tx_buffer, Buffer rx_buffer, int event) {
        cs = 1;
        printf("Long transfer DONE, event is %d\r\n", event);
        compare_buffers((uint8_t*)rx_buffer.buf, (uint8_t*)tx_buffer.buf, rx_buffer.length);
        printf("**** Test done ****\r\n");
    }

private:
    SPI spi;
    DigitalOut cs;
    uint8_t tx_buf[LONG_XFR];
    uint8_t rx_buf[LONG_XFR];
};

void app_start(int, char*[]) {
    static SPITest test;
    Scheduler::postCallback(mbed::util::FunctionPointer0<void>(&test, &SPITest::start).bind());
}

#else
void app_start(int, char*[]) {
}
#endif

