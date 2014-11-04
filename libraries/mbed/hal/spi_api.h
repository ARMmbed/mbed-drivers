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
#ifndef MBED_SPI_API_H
#define MBED_SPI_API_H

#include "device.h"
#include "dma_api.h"
#include "buffer.h"

#if DEVICE_SPI

#ifdef __cplusplus
extern "C" {
#endif

#define SPI_EVENT_COMPLETE (1 << 0)

typedef struct {
    struct spi_s    spi;
    struct buffer_s tx_buff;
    struct buffer_s rx_buff;
} spi_t;

void spi_init         (spi_t *obj, PinName mosi, PinName miso, PinName sclk, PinName ssel);
void spi_free         (spi_t *obj);
void spi_format       (spi_t *obj, int bits, int mode, int slave);
void spi_frequency    (spi_t *obj, int hz);
int  spi_master_write (spi_t *obj, int value);
int  spi_slave_receive(spi_t *obj);
int  spi_slave_read   (spi_t *obj);
void spi_slave_write  (spi_t *obj, int value);
int  spi_busy         (spi_t *obj);

/* asynch */

// Enable events
void spi_enable_event(spi_t *obj, uint32_t event, uint8_t enable);

// Enable SPI interrupts
void spi_enable_interrupt(spi_t *obj, uint32_t handler, uint8_t enable);

// Initiate the transfer
void spi_master_transfer(spi_t *obj, void *rxdata, void *txdata, int length, void* cb, DMA_USAGE_Enum hint);

// write data until hw buffer is full
int spi_master_write_asynch(spi_t *obj);

// Read available data
int spi_master_read_asynch(spi_t *obj);

// Synch handler
void spi_irq_handler(spi_t *obj);

// Asynch irq handler
uint32_t spi_irq_handler_asynch(spi_t *obj);

// returns if spi transaction is ongoing
uint8_t spi_active(spi_t *obj);

// Initialiaze tx and rx buffers
void spi_buffer_set(spi_t *obj, void *tx, uint32_t tx_length, void *rx, uint32_t rx_length);

#ifdef __cplusplus
}
#endif

#endif

#endif
