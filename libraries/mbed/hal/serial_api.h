/* mbed Microcontroller Library
 * Copyright (c) 2006-2013 ARM Limited
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
#ifndef MBED_SERIAL_API_H
#define MBED_SERIAL_API_H

#include "device.h"
#include "buffer.h"
#include "dma_api.h"

#if DEVICE_SERIAL

#define SERIAL_EVENT_TX_SHIFT (2)
#define SERIAL_EVENT_RX_SHIFT (8)

#define SERIAL_EVENT_TX_MASK  (0x00FC)
#define SERIAL_EVENT_RX_MASK  (0x3F00)

#define SERIAL_EVENT_ERROR       (1 << 1)
#define SERIAL_EVENT_TX_COMPLETE (1 << (SERIAL_EVENT_TX_SHIFT + 0))

#define SERIAL_EVENT_RX_COMPLETE      (1 << (SERIAL_EVENT_RX_SHIFT + 0))
#define SERIAL_EVENT_RX_OVERRUN_ERROR (1 << (SERIAL_EVENT_RX_SHIFT + 1))
#define SERIAL_EVENT_RX_FRAMING_ERROR (1 << (SERIAL_EVENT_RX_SHIFT + 2))
#define SERIAL_EVENT_RX_PARITY_ERROR  (1 << (SERIAL_EVENT_RX_SHIFT + 3))

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    ParityNone = 0,
    ParityOdd = 1,
    ParityEven = 2,
    ParityForced1 = 3,
    ParityForced0 = 4
} SerialParity;

typedef enum {
    RxIrq,
    TxIrq
} SerialIrq;

typedef enum {
    FlowControlNone,
    FlowControlRTS,
    FlowControlCTS,
    FlowControlRTSCTS
} FlowControl;

typedef void (*uart_irq_handler)(uint32_t id, SerialIrq event);

typedef struct {
    struct serial_s serial;
    struct buffer_s tx_buff;
    struct buffer_s rx_buff;
} serial_t;

void serial_init       (serial_t *obj, PinName tx, PinName rx);
void serial_free       (serial_t *obj);
void serial_baud       (serial_t *obj, int baudrate);
void serial_format     (serial_t *obj, int data_bits, SerialParity parity, int stop_bits);

void serial_irq_handler(serial_t *obj, uart_irq_handler handler, uint32_t id);
void serial_irq_set    (serial_t *obj, SerialIrq irq, uint32_t enable);

int  serial_getc       (serial_t *obj);
void serial_putc       (serial_t *obj, int c);
int  serial_readable   (serial_t *obj);
int  serial_writable   (serial_t *obj);
void serial_clear      (serial_t *obj);

void serial_break_set  (serial_t *obj);
void serial_break_clear(serial_t *obj);

void serial_pinout_tx(PinName tx);

void serial_set_flow_control(serial_t *obj, FlowControl type, PinName rxflow, PinName txflow);

// Asynch

// Start tx transfer
int serial_start_write_asynch(serial_t *obj, void* cb, DMA_USAGE_Enum hint);

// Start rx transfer
void serial_start_read_asynch(serial_t *obj, void* cb, DMA_USAGE_Enum hint);

// Enable tx event
void serial_tx_enable_event(serial_t *obj, uint32_t event, uint8_t enable);

// Enable rx event
void serial_rx_enable_event(serial_t *obj, uint32_t event, uint8_t enable);

// Set tx buffer
void serial_tx_buffer_set(serial_t *obj, void *tx, uint32_t tx_length);

// Set rx buffer
void serial_rx_buffer_set(serial_t *obj, void *rx, uint32_t tx_length);

// Return true if tx transfer is ongoing
uint8_t serial_tx_active(serial_t *obj);

// Return true if rx transfer is ongoing
uint8_t serial_rx_active(serial_t *obj);

// tx isr handler
uint32_t serial_tx_irq_handler_asynch(serial_t *obj);

// rx isr handler
uint32_t serial_rx_irq_handler_asynch(serial_t *obj);

// Abort tx transfer
void serial_tx_abort_asynch(serial_t *obj);

// Abort rx transfer
void serial_rx_abort_asynch(serial_t *obj);

// Enable interrupt for tx
void serial_write_enable_interrupt(serial_t *obj, uint32_t address, uint8_t enable);

// Enable interrupt for rx
void serial_read_enable_interrupt(serial_t *obj, uint32_t address, uint8_t enable);

#ifdef __cplusplus
}
#endif

#endif

#endif
