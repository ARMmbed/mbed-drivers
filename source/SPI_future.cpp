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
#include "minar/minar.h"
#include "target_config.h"

#if DEVICE_SPI

namespace mbed {

#if DEVICE_SPI_ASYNCH && TRANSACTION_QUEUE_SIZE_SPI
CircularBuffer<SPI::transaction_t, TRANSACTION_QUEUE_SIZE_SPI> SPI::_transaction_buffer;
#endif

class SPI_physical : public SPI_interface {

}

static SPI_interface * _interfaces[MODULE_SIZE_SPI];

SPI_interface * SPI::get_instance(PinName mosi, PinName miso, PinName sclk)
{
    uint32_t spi_mosi = pinmap_peripheral(mosi, PinMap_SPI_MOSI);
    uint32_t spi_miso = pinmap_peripheral(miso, PinMap_SPI_MISO);
    uint32_t spi_sclk = pinmap_peripheral(sclk, PinMap_SPI_SCLK);
    uint32_t spi_data = pinmap_merge(spi_mosi, spi_miso);
    uint32_t instance = pinmap_merge(spi_data, spi_sclk);
    MBED_ASSERT((int)instance != NC);

    if (instance > MODULE_SIZE_SPI)
    {
        return NULL;
    }

    if (_interfaces[instance] == NULL) {
        _interfaces[instance] = new SPI_physical(mosi, miso, sclk);
    }
    
    return _interfaces[instance];
}

SPI::SPI(PinName mosi, PinName miso, PinName sclk, PinName cs) :
    _instance(get_instance(mosi, miso, sclk))
{
    _xd
        .frequency(100000)
        .irq_callback(event_callback_t(NULL))
        .callback(event_callback_t(NULL))
        .tx_buffer(NULL, 0)
        .rx_buffer(NULL, 0)
        .event_mask(SPI_EVENT_ALL)
        .cs_pin(cs)
        .cs_setup_time(0)
        .cs_hold_time(0)
        .mode(8, 0, SPI_MSB)
        .dma_usage(DMA_USAGE_NEVER);
}

void SPI::format(int bits, int mode, spi_bitorder_t order) {
    _xd.mode(bits, mode, order);
}

void SPI::frequency(int hz) {
    _xd.frequency(hz);
}

int SPI::post_transfer(const SPI_xfer_data & tp) {
    return _instance->post_transfer(tp);
}

void SPI::abort_transfer()
{
    _instance->abort_transfer();
}

void SPI::clear_transfer_buffer()
{
    _instance->clear_transfer_buffer();
}

void SPI::abort_all_transfers()
{
    _instance->abort_all_transfers();
}

void SPI::set_dma_usage(DMAUsage usage)
{
    _xd.dma_usage(usage);
}

int SPI::queue_transfer(const Buffer& tx, const Buffer& rx, const event_callback_t& callback, int event) {
#if TRANSACTION_QUEUE_SIZE_SPI
    transaction_data_t t;

    t.tx_buffer = tx;
    t.rx_buffer = rx;
    t.event = event;
    t.callback = callback;
    transaction_t transaction(this, t);
    if (_transaction_buffer.full()) {
        return -1; // the buffer is full
    } else {
        _transaction_buffer.push(transaction);
        return 0;
    }
#else
    return -1;
#endif
}

void SPI::start_transfer(const Buffer& tx, const Buffer& rx, const event_callback_t& callback, int event) {
    aquire();
    _current_transaction.callback = callback;
    _current_transaction.tx_buffer = tx;
    _current_transaction.rx_buffer = rx;
    _irq.callback(&SPI::irq_handler_asynch);
    spi_master_transfer(&_spi, tx.buf, tx.length, rx.buf, rx.length, _irq.entry(), event , _usage);
}

#if TRANSACTION_QUEUE_SIZE_SPI

void SPI::start_transaction(transaction_data_t *data)
{
    start_transfer(data->tx_buffer, data->rx_buffer, data->callback, data->event);
}

void SPI::dequeue_transaction()
{
    transaction_t t;
    if (_transaction_buffer.pop(t)) {
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
        minar::Scheduler::postCallback(_current_transaction.callback.bind(_current_transaction.tx_buffer, _current_transaction.rx_buffer, event & SPI_EVENT_ALL));
    }
#if TRANSACTION_QUEUE_SIZE_SPI
    if (event & (SPI_EVENT_ALL | SPI_EVENT_INTERNAL_TRANSFER_COMPLETE)) {
        // SPI peripheral is free (event happend), dequeue transaction
        dequeue_transaction();
    }
#endif
}

SPI::SPI_xfer_data::SPI_xfer_data():
    _freq(0),
    _irqCallback(),
    _callback(),
    _tx(NULL),
    _rx(NULL),
    _tlen(0),
    _rlen(0),
    _eventMask(-1),
    _cs()
{
}
SPI_xfer_data & SPI::SPI_xfer_data::frequency(uint32_t freq)
{
    _freq = freq;
    return *this;
}
SPI_xfer_data & SPI::SPI_xfer_data::irq_callback(event_callback_t irqCallback)
{
    _irqCallback = irqCallback;
    return *this;
}
SPI_xfer_data & SPI::SPI_xfer_data::callback(event_callback_t callback)
{
    _callback = callback;
    return *this;
}
SPI_xfer_data & SPI::SPI_xfer_data::tx_buffer(void * buf, size_t length)
{
    _tx = buf;
    _tlen = length;
    return *this;
}
SPI_xfer_data & SPI::SPI_xfer_data::rx_buffer(void * buf, size_t length)
{
    _rx = buf;
    _rlen = length;
    return *this;
}
SPI_xfer_data & SPI::SPI_xfer_data::event_mask(int mask)
{
    _eventMask = mask;
    return *this;
}
SPI_xfer_data & SPI::SPI_xfer_data::cs_pin(PinName cs)
{
    _cs = cs;
    return *this;
}

SPI::SPI_xfer_data::~SPI_xfer_data() {
    if(_spi && !_posted) {
        int err = _spi->transfer(_tx,_tlen,_rx,_rlen,_callback,_eventMask);
        if (err && _callback) {
            minar::Scheduler::postCallback(_callback.bind(SPI_EVENT_ERROR));
        }
    }
}


#endif

} // namespace mbed

#endif
