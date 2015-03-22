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
#include "SPI.h"

#if DEVICE_SPI

namespace mbed {

SPI::SPI(PinName mosi, PinName miso, PinName sclk, PinName _unused) :
        _spi(),
#if DEVICE_SPI_ASYNCH
        _irq(this),
        _user_callback(0),
        _usage(DMA_USAGE_NEVER),
#endif
        _bits(8),
        _mode(0),
        _hz(1000000) {
    spi_init(&_spi, mosi, miso, sclk, NC);
    spi_format(&_spi, _bits, _mode, 0);
    spi_frequency(&_spi, _hz);
}

void SPI::format(int bits, int mode) {
    _bits = bits;
    _mode = mode;
    SPI::_owner = NULL; // Not that elegant, but works. rmeyer
    aquire();
}

void SPI::frequency(int hz) {
    _hz = hz;
    SPI::_owner = NULL; // Not that elegant, but works. rmeyer
    aquire();
}

SPI* SPI::_owner = NULL;

// ignore the fact there are multiple physical spis, and always update if it wasnt us last
void SPI::aquire() {
     if (_owner != this) {
        spi_format(&_spi, _bits, _mode, 0);
        spi_frequency(&_spi, _hz);
        _owner = this;
    }
}

int SPI::write(int value) {
    aquire();
    return spi_master_write(&_spi, value);
}

#if DEVICE_SPI_ASYNCH

int SPI::transfer(uint8_t *tx_buffer, int tx_length, uint8_t *rx_buffer, int rx_length, const callback_t& callback, int event)
{
    if (spi_active(&_spi)) {
        return queue_transfer(tx_buffer, tx_length, rx_buffer, rx_length, 8, callback, event);
    }
    start_transfer(callback, tx_buffer, tx_length, rx_buffer, rx_length, 8, event);
    return 0;
}

int SPI::transfer(uint16_t *tx_buffer, int tx_length, uint16_t *rx_buffer, int rx_length, const callback_t& callback, int event)
{
    if (spi_active(&_spi)) {
        return queue_transfer(tx_buffer, tx_length, rx_buffer, rx_length, 16, callback, event);
    }
    start_transfer(callback, tx_buffer, tx_length, rx_buffer, rx_length, 16, event);
    return 0;
}

int SPI::transfer(uint32_t *tx_buffer, int tx_length, uint32_t *rx_buffer, int rx_length, const callback_t& callback, int event)
{
    if (spi_active(&_spi)) {
        return queue_transfer(tx_buffer, tx_length, rx_buffer, rx_length, 32, callback, event);
    }
    start_transfer(callback, tx_buffer, tx_length, rx_buffer, rx_length, 32, event);
    return 0;
}

void SPI::start_transfer(const callback_t& callback, void *tx, int tx_length, void *rx, int rx_length, unsigned char bit_width, int event)
{
    aquire();

    _user_callback = callback;
    _irq.callback(&SPI::irq_handler_asynch);
    spi_master_transfer(&_spi, tx, tx_length, rx, rx_length, bit_width, _irq.entry(), event , _usage);
}

int SPI::set_dma_usage(DMAUsage usage)
{
    if (spi_active(&_spi)) {
        return -1;
    }
    _usage = usage;
    return  0;
}

int SPI::queue_transfer(void *tx_buffer, int tx_length, void *rx_buffer, int rx_length, unsigned char bit_width, const callback_t& callback, int event)
{
    spi_transaction_t t;

    t.tx_buffer = tx_buffer;
    t.tx_length = tx_length;
    t.rx_buffer = rx_buffer;
    t.rx_length = rx_length;
    t.callback = callback;
    t.event = event;
    t.width = bit_width;
    Transaction<SPI, spi_transaction_t> transaction(this, t);
    uint8_t index = spi_get_module(&_spi);
    if (!_spi_module.push(transaction, index)) {
        return -1; // the buffer is full
    } else {
        return 0;
    }
}

void SPI::start_transaction(spi_transaction_t *data)
{
    start_transfer(data->callback, data->tx_buffer, data->tx_length, data->rx_buffer, data->rx_length, data->width, data->event);
}

void SPI::irq_handler_asynch(void)
{
    int event = spi_irq_handler_asynch(&_spi);
    if (_user_callback && (event & SPI_EVENT_ALL)) {
        yottos::Scheduler::instance()->postCallback(_user_callback.arg(event));
    }

    if (event & SPI_EVENT_INTERNAL_TRANSFER_COMPLETE) {
        // SPI peripheral is free, dequeue transaction
        Transaction<SPI, spi_transaction_t> t;
        uint8_t index = spi_get_module(&_spi);
        if (_spi_module.pop(t, index)) {
            SPI* obj = t.get_object();
            spi_transaction_t* data = t.get_transaction();
            obj->start_transaction(data);
        }
    }
}

#endif

} // namespace mbed

#endif
