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
#include <math.h>
#include "mbed_assert.h"

#include "spi_api.h"

#if DEVICE_SPI

#include "cmsis.h"
#include "pinmap.h"
#include "mbed_error.h"
#include "fsl_clock_manager.h"
#include "fsl_dspi_hal.h"
#include "PeripheralPins.h"

void spi_init(spi_t *obj, PinName mosi, PinName miso, PinName sclk, PinName ssel) {
    // determine the SPI to use
    uint32_t spi_mosi = pinmap_peripheral(mosi, PinMap_SPI_MOSI);
    uint32_t spi_miso = pinmap_peripheral(miso, PinMap_SPI_MISO);
    uint32_t spi_sclk = pinmap_peripheral(sclk, PinMap_SPI_SCLK);
    uint32_t spi_ssel = pinmap_peripheral(ssel, PinMap_SPI_SSEL);
    uint32_t spi_data = pinmap_merge(spi_mosi, spi_miso);
    uint32_t spi_cntl = pinmap_merge(spi_sclk, spi_ssel);

    obj->spi.instance = pinmap_merge(spi_data, spi_cntl);
    MBED_ASSERT((int)obj->spi.instance != NC);

    CLOCK_SYS_EnableSpiClock(obj->spi.instance);
    uint32_t spi_address[] = SPI_BASE_ADDRS;
    DSPI_HAL_Init(spi_address[obj->spi.instance]);
    DSPI_HAL_Disable(spi_address[obj->spi.instance]);
    // set default format and frequency
    if (ssel == NC) {
        spi_format(obj, 8, 0, 0);  // 8 bits, mode 0, master
    } else {
        spi_format(obj, 8, 0, 1);  // 8 bits, mode 0, slave
    }
    DSPI_HAL_SetDelay(spi_address[obj->spi.instance], kDspiCtar0, 0, 0, kDspiPcsToSck);
    spi_frequency(obj, 1000000);

    DSPI_HAL_Enable(spi_address[obj->spi.instance]);
    DSPI_HAL_StartTransfer(spi_address[obj->spi.instance]);

    // pin out the spi pins
    pinmap_pinout(mosi, PinMap_SPI_MOSI);
    pinmap_pinout(miso, PinMap_SPI_MISO);
    pinmap_pinout(sclk, PinMap_SPI_SCLK);
    if (ssel != NC) {
        pinmap_pinout(ssel, PinMap_SPI_SSEL);
    }
}

void spi_free(spi_t *obj) {
    // [TODO]
}
void spi_format(spi_t *obj, int bits, int mode, int slave) {
    dspi_data_format_config_t config = {0};
    config.bitsPerFrame = (uint32_t)bits;
    config.clkPolarity = (mode & 0x2) ? kDspiClockPolarity_ActiveLow : kDspiClockPolarity_ActiveHigh;
    config.clkPhase = (mode & 0x1) ? kDspiClockPhase_SecondEdge : kDspiClockPhase_FirstEdge;
    config.direction = kDspiMsbFirst;
    uint32_t spi_address[] = SPI_BASE_ADDRS;
    dspi_status_t result = DSPI_HAL_SetDataFormat(spi_address[obj->spi.instance], kDspiCtar0, &config);
    if (result != kStatus_DSPI_Success) {
        error("Failed to configure SPI data format");
    }

    if (slave) {
        DSPI_HAL_SetMasterSlaveMode(spi_address[obj->spi.instance], kDspiSlave);
    } else {
        DSPI_HAL_SetMasterSlaveMode(spi_address[obj->spi.instance], kDspiMaster);
    }
}

void spi_frequency(spi_t *obj, int hz) {
    uint32_t busClock;
    CLOCK_SYS_GetFreq(kBusClock, &busClock);
    uint32_t spi_address[] = SPI_BASE_ADDRS;
    DSPI_HAL_SetBaudRate(spi_address[obj->spi.instance], kDspiCtar0, (uint32_t)hz, busClock);
}

static inline int spi_writeable(spi_t * obj) {
    uint32_t spi_address[] = SPI_BASE_ADDRS;
    return DSPI_HAL_GetStatusFlag(spi_address[obj->spi.instance], kDspiTxFifoFillRequest);
}

static inline int spi_readable(spi_t * obj) {
    uint32_t spi_address[] = SPI_BASE_ADDRS;
    return DSPI_HAL_GetStatusFlag(spi_address[obj->spi.instance], kDspiRxFifoDrainRequest);
}

int spi_master_write(spi_t *obj, int value) {
    uint32_t spi_address[] = SPI_BASE_ADDRS;

    // wait tx buffer empty
    while(!spi_writeable(obj));
    dspi_command_config_t command = {0};
    command.isEndOfQueue = true;
    command.isChipSelectContinuous = 0;
    DSPI_HAL_WriteDataMastermode(spi_address[obj->spi.instance], &command, (uint16_t)value);
    DSPI_HAL_ClearStatusFlag(spi_address[obj->spi.instance], kDspiTxFifoFillRequest);

    // wait rx buffer full
    while (!spi_readable(obj));
    DSPI_HAL_ClearStatusFlag(spi_address[obj->spi.instance], kDspiRxFifoDrainRequest);
    return DSPI_HAL_ReadData(spi_address[obj->spi.instance]) & 0xff;
}

int spi_slave_receive(spi_t *obj) {
    return spi_readable(obj);
}

int spi_slave_read(spi_t *obj) {
    DSPI_HAL_ClearStatusFlag(obj->spi.instance, kDspiRxFifoDrainRequest);
    uint32_t spi_address[] = SPI_BASE_ADDRS;
    return DSPI_HAL_ReadData(spi_address[obj->spi.instance]);
}

void spi_slave_write(spi_t *obj, int value) {
    while (!spi_writeable(obj));
    uint32_t spi_address[] = SPI_BASE_ADDRS;
    DSPI_HAL_WriteDataSlavemode(spi_address[obj->spi.instance], (uint32_t)value);
}

void spi_enable_event(spi_t *obj, uint32_t event, uint8_t enable)
{

}

void spi_enable_interrupt(spi_t *obj, uint32_t handler, uint8_t enable)
{

}

void spi_master_transfer(spi_t *obj, void *rxdata, void *txdata, int length, void* cb, DMA_USAGE_Enum hint)
{
    if (hint != DMA_USAGE_NEVER && obj->spi.dma_state == DMA_USAGE_ALLOCATED) {
        // setup dma done, activate
    } else if (hint == DMA_USAGE_NEVER) {
        obj->spi.dma_state = DMA_USAGE_NEVER;
        // use IRQ
    } else {
        // setup and activate
    }
}

int spi_master_write_asynch(spi_t *obj)
{
    return 0;
}

int spi_master_read_asynch(spi_t *obj)
{
    return 0;
}

void spi_irq_handler(spi_t *obj)
{

}

uint32_t spi_irq_handler_asynch(spi_t *obj)
{
    if (obj->spi.dma_state == DMA_USAGE_ALLOCATED || obj->spi.dma_state == DMA_USAGE_TEMPORARY_ALLOCATED) {
        /* DMA implementation */
    } else {
        // IRQ
    }
    return 0;
}

uint8_t spi_active(spi_t *obj)
{
    switch(obj->spi.dma_state) {
        case DMA_USAGE_TEMPORARY_ALLOCATED:
            return 1;
        case DMA_USAGE_ALLOCATED:
            /* Check whether the allocated DMA channel is active */
            return 0;
        default:
            /* Check whether interrupt for spi is enabled */
            return 0; // TODO implement check if interrupt is enabled
    }
}

void spi_buffer_set(spi_t *obj, void *tx, uint32_t tx_length, void *rx, uint32_t rx_length)
{

}


#endif
