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
#include <TestHarness.h>
#include <mbed.h>
#include <SPI.h>
#include <spi_api.h>
#include <stdio.h>

/* Serial asynch Loopback test
   Using 8bit transfer,
*/

// Device config
#if defined(TARGET_K64F)
#define TEST_TX_PIN PTC17
#define TEST_RX_PIN PTC16
#else

#error Target not supported
#endif

// Test config
#define SHORT_XFR 3
#define LONG_XFR 16
#define TEST_BYTE_TX_BASE 0x55
#define TEST_BYTE_RX      0x5A

volatile uint32_t tx_event_flag;
volatile bool tx_complete;

volatile uint32_t rx_event_flag;
volatile bool rx_complete;

void cb_tx_done(uint32_t event)
{
    tx_complete = true;
    tx_event_flag = event;
}

void cb_rx_done(uint32_t event)
{
    rx_complete = true;
    rx_event_flag = event;
}

TEST_GROUP(Serial_Asynchronous_Loopback)
{
    uint8_t tx_buf[LONG_XFR];
    uint8_t rx_buf[LONG_XFR];
    Serial *obj;

    void setup()
    {
        obj = new Serial(TEST_TX_PIN, TEST_RX_PIN);
        tx_complete = false;
        tx_event_flag = 0;
        rx_complete = false;
        rx_event_flag = 0;

        // Set the default value of tx_buf
        for (uint32_t i = 0; i < sizeof(tx_buf); i++) {
            tx_buf[i] = i + TEST_BYTE_TX_BASE;
        }
        memset(rx_buf, TEST_BYTE_RX, sizeof(rx_buf));
    }

    void teardown()
    {
        delete obj;
        obj = NULL;
    }

    uint32_t cmpnbufc(uint8_t expect, uint8_t *actual, uint32_t offset, uint32_t end, const char *file, uint32_t line)
    {
        uint32_t i;
        for (i = offset; i < end; i++){
            if (expect != actual[i]) {
                break;
            }
        }
        if (i < end) {
            CHECK_EQUAL_LOCATION((int)expect, (int)actual[i], file, line);
        }
        CHECK_EQUAL_LOCATION(end, i, file, line);
        return i;
    }

    uint32_t cmpnbuf(uint8_t *expect, uint8_t *actual, uint32_t offset, uint32_t end, const char *file, uint32_t line)
    {
        uint32_t i;
        for (i = offset; i < end; i++){
            if (expect[i] != actual[i]) {
                break;
            }
        }
        if (i < end) {
            CHECK_EQUAL_LOCATION((int)expect[i], (int)actual[i], file, line);
        }
        CHECK_EQUAL_LOCATION(end, i, file, line);
        return i;
    }

};

TEST(Serial_Asynchronous_Loopback, short_tx_0_rx)
{
    int rc;
    rc = obj->write((void *)tx_buf, SHORT_XFR, -1, cb_tx_done);
    CHECK_EQUAL(0, rc);

    while (!tx_complete);

    CHECK_EQUAL(SERIAL_EVENT_TX_COMPLETE, tx_event_flag);
    // rx buffer unchanged
    cmpnbufc(TEST_BYTE_RX, rx_buf, 0, sizeof(rx_buf), __FILE__, __LINE__);
}

TEST(Serial_Asynchronous_Loopback, short_tx_short_rx)
{
    int rc;
    obj->read((void *)rx_buf, SHORT_XFR, -1, cb_rx_done);
    rc = obj->write((void *)tx_buf, SHORT_XFR, -1, cb_tx_done);
    CHECK_EQUAL(0, rc);

    while ((!tx_complete) || (!rx_complete));

    CHECK_EQUAL(SERIAL_EVENT_TX_COMPLETE, tx_event_flag);
    CHECK_EQUAL(SERIAL_EVENT_RX_COMPLETE, rx_event_flag);

    // Check that the receive buffer contains the fill byte.
    cmpnbuf(tx_buf, rx_buf, 0, SHORT_XFR, __FILE__, __LINE__);
    // Check that remaining portion of the receive buffer contains the rx test byte
    cmpnbufc(TEST_BYTE_RX, rx_buf, SHORT_XFR, sizeof(rx_buf), __FILE__, __LINE__);
}

TEST(Serial_Asynchronous_Loopback, long_tx_long_rx)
{
    int rc;
    obj->read((void *)rx_buf, LONG_XFR, -1, cb_rx_done);
    rc = obj->write((void *)tx_buf, LONG_XFR, -1, cb_tx_done);
    CHECK_EQUAL(0, rc);

    while ((!tx_complete) || (!rx_complete));

    CHECK_EQUAL(SERIAL_EVENT_TX_COMPLETE, tx_event_flag);
    CHECK_EQUAL(SERIAL_EVENT_RX_COMPLETE, rx_event_flag);

    // Check that the receive buffer contains the fill byte.
    cmpnbuf(tx_buf, rx_buf, 0, LONG_XFR, __FILE__, __LINE__);
    // Check that remaining portion of the receive buffer contains the rx test byte
    cmpnbufc(TEST_BYTE_RX, rx_buf, LONG_XFR, sizeof(rx_buf), __FILE__, __LINE__);
}

// TODO test case for abort_read and abort_write
