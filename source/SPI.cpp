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
#include "mbed-drivers/SPI.h"
#include "minar/minar.h"
#include "mbed-drivers/mbed_assert.h"
#include "core-util/CriticalSectionLock.h"

#if DEVICE_SPI
namespace mbed {

using namespace util;

#if DEVICE_SPI_ASYNCH && TRANSACTION_QUEUE_SIZE_SPI
CircularBuffer<SPI::transaction_t, TRANSACTION_QUEUE_SIZE_SPI> SPI::_transaction_buffer;
#endif

SPI::SPI(PinName mosi, PinName miso, PinName sclk) :
        _spi(),
#if DEVICE_SPI_ASYNCH
        _irq(this),
        _usage(DMA_USAGE_NEVER),
#endif
        _bits(8),
        _mode(0),
        _order(SPI_MSB),
        _hz(1000000) {
    spi_init(&_spi, mosi, miso, sclk);
    spi_format(&_spi, _bits, _mode, _order);
    spi_frequency(&_spi, _hz);
}

void SPI::format(int bits, int mode, spi_bitorder_t order) {
    _bits = bits;
    _mode = mode;
    _order = order;
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
        spi_format(&_spi, _bits, _mode, _order);
        spi_frequency(&_spi, _hz);
        _owner = this;
    }
}

int SPI::write(int value) {
    aquire();
    return spi_master_write(&_spi, value);
}

#if DEVICE_SPI_ASYNCH

int SPI::transfer(const SPI::SPITransferAdder &td)
{
    bool queue;
    {
        CriticalSectionLock lock;
        queue = _busy;
        _busy = true;
    }
    if (queue || spi_active(&_spi)) {
        return queue_transfer(td._td);
    }
    start_transfer(td._td);
    return 0;
}

void SPI::abort_transfer()
{
    spi_abort_asynch(&_spi);
#if TRANSACTION_QUEUE_SIZE_SPI
    dequeue_transaction();
#endif
}


void SPI::clear_transfer_buffer()
{
#if TRANSACTION_QUEUE_SIZE_SPI
    _transaction_buffer.reset();
#endif
}

void SPI::abort_all_transfers()
{
    clear_transfer_buffer();
    abort_transfer();
}

int SPI::set_dma_usage(DMAUsage usage)
{
    if (spi_active(&_spi)) {
        return -1;
    }
    _usage = usage;
    return  0;
}

int SPI::queue_transfer(const transaction_data_t &td)
{
#if TRANSACTION_QUEUE_SIZE_SPI
    CriticalSectionLock lock;
    int result;

    transaction_t transaction(this, td);
    if (_transaction_buffer.full()) {
        result = -1;
    } else {
        _transaction_buffer.push(transaction);
        result = 0;
    }
    return result;
#else
    return -1;
#endif
}

void SPI::start_transfer(const transaction_data_t &td)
{
    aquire();
    _current_transaction = td;
    _irq.callback(&SPI::irq_handler_asynch);
    spi_master_transfer(&_spi, td.tx_buffer.buf, td.tx_buffer.length, td.rx_buffer.buf, td.rx_buffer.length,
            _irq.entry(), td.event, _usage);
}

#if TRANSACTION_QUEUE_SIZE_SPI

void SPI::start_transaction(transaction_data_t *data)
{
    start_transfer(*data);
}

void SPI::dequeue_transaction()
{
    transaction_t t;
    bool dequeued;
    {
        CriticalSectionLock lock;
        dequeued = _transaction_buffer.pop(t);
        _busy = dequeued;
    }

    if (dequeued) {
        SPI* obj = t.get_object();
        transaction_data_t* data = t.get_transaction();
        obj->start_transaction(data);
    }
}

#endif

void SPI::irq_handler_asynch(void)
{
    int event = spi_irq_handler_asynch(&_spi);
    if (_current_transaction.callback && (event & SPI_EVENT_ALL)) {
        minar::Scheduler::postCallback(
                _current_transaction.callback.bind(_current_transaction.tx_buffer, _current_transaction.rx_buffer,
                        event & SPI_EVENT_ALL));
    }
#if TRANSACTION_QUEUE_SIZE_SPI
    if (event & (SPI_EVENT_ALL | SPI_EVENT_INTERNAL_TRANSFER_COMPLETE)) {
        dequeue_transaction();
    }
#endif
}

SPI::SPITransferAdder::SPITransferAdder(SPI *owner) :
        _applied(false), _rc(0), _owner(owner)
{
    _td.tx_buffer.length = 0;
    _td.rx_buffer.length = 0;
    _td.callback = event_callback_t((void (*)(Buffer, Buffer, int))NULL);
}
const SPI::SPITransferAdder & SPI::SPITransferAdder::operator =(const SPI::SPITransferAdder &a)
{
    _td = a._td;
    _owner = a._owner;
    _applied = 0;
    return *this;
}
SPI::SPITransferAdder::SPITransferAdder(const SPITransferAdder &a)
{
    *this = a;
}
SPI::SPITransferAdder & SPI::SPITransferAdder::tx(void *txBuf, size_t txSize)
{
    MBED_ASSERT(!_td.tx_buffer.length);
    _td.tx_buffer.buf = txBuf;
    _td.tx_buffer.length = txSize;
    return *this;
}
SPI::SPITransferAdder & SPI::SPITransferAdder::rx(void *rxBuf, size_t rxSize)
{
    MBED_ASSERT(!_td.rx_buffer.length);
    _td.rx_buffer.buf = rxBuf;
    _td.rx_buffer.length = rxSize;
    return *this;
}
SPI::SPITransferAdder & SPI::SPITransferAdder::callback(const event_callback_t &cb, int event)
{
    MBED_ASSERT(!_td.callback);
    _td.callback = cb;
    _td.event = event;
    return *this;
}
int SPI::SPITransferAdder::apply()
{
    if (!_applied) {
        _applied = true;
        _rc = _owner->transfer(*this);
    }
    return _rc;
}
SPI::SPITransferAdder::~SPITransferAdder()
{
    apply();
}

SPI::SPITransferAdder SPI::transfer()
{
    SPITransferAdder a(this);
    return a;
}
#endif

} // namespace mbed

#endif
