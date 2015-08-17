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
typedef CircularBuffer<SPI_xfer_data,TRANSACTION_QUEUE_SIZE_SPI> SPI_xfer_Queue_t
#endif


class SPI_physical : public SPI_interface {
    SPI_physical(PinName mosi, PinName miso, PinName sclk):
        _irq(this)
    {
        _irq.callback(&SPI_physical::SPIirq)
        spi_init(&_spi, mosi, miso, sclk);
    }
    /** Start non-blocking SPI transfer.
     * @param tp The transfer Parameters descriptor object
     * @return Zero if the transfer has started, or an error code if
     *         a problem occurred.
     */
    int post_transfer(const SPI_xfer_data & tp)
    {
        _queue.push(tp);
        //TODO: Critical Section
        if (!_currentTransfer) {
            preXfer();
        }
    }
protected:
    /* SPI State machine functions */

    void preXfer()
    {
        bool dataPosted = false;
        /* If a transfer is not active: */
        if (_currentTransfer)
        {
            if (_queue.empty()){
                // TODO: Halt the SPI peripheral, clock gate, power gate
                return;
            }
            /* Dequeue a transaction */
            _queue.pop(_currentTransfer);
            /* Mark this pass as the first call for the
            _transferFirstXfer
            /* If there is a user pre-xfer callback: */
            if (_currentTransfer._irqPreCallback) {
                /* call the user pre-xfer callback */
                _currentTransfer._irqPreCallback(&_currentTransfer);
            }
            spi_format(&_spi,
                _currentTransfer._bits,
                _currentTransfer._mode,
                _currentTransfer._order,
                0);
            spi_frequency(&_spi, _currentTransfer._hz);
            _currentSSEL = _currentTransfer._cs;
            _SSELActiveHigh = _currentTransfer._csActiveHigh;
            if (_currentSSEL != NC) {
                /* If CS can be managed by the SPI peripheral */
                _SSELManaged = (pinmap_peripheral(PinMap_SPI_SSEL, _currentSSEL) != NC);
                if (_SSELManaged) {
                    /* Configure the CS pin */
                    pinmap_pinout(_currentSSEL, PinMap_SPI_SSEL);
                } else {
                    /* Configure the CS pin as GPOut */
                    gpio_init_out(&_ssel, _currentSSEL);
                    /* set GPIO active (CS) */
                    gpio_write(&_ssel, _SSELActiveHigh);
                }
                // TODO:

                ///* If the transaction requires autotoggle and the peripheral does not support it:
                //if (spi_can_toggle(&_spi, _currentSSEL)) {
                //    /* Disable FIFOs */
                //    spi_fifo_set(&_spi, false);
                //    /* Enable interrupt on every word */
                //    spi_irq_set(&_spi, ???);
                //}
            }
            if (!_SSELManaged && _currentTransfer._cs_setup_time != 0) {
                minar::Scheduler::postCallback(event_callback_t(this, &SPI_physical::sendData).bind())
                        .delay(_currentTransfer._cs_setup_time)
                        .tolerance(0);
                dataPosted = true;
            }
        } else {
            /* If CS is not managed: */
            if (!_SSELManaged) {
                /* If CS is not NC: */
                if (_currentSSEL != NC) {
                    /* set GPIO active (CS) */
                    gpio_write(&_ssel, _SSELActiveHigh);
                }
                /* If the setup delay is not 0: */
                if (_currentTransfer._cs_setup_time != 0) {
                    /* post sendData() to execute (setup delay) from now */
                    minar::Scheduler::postCallback(event_callback_t(this, &SPI_physical::sendData).bind())
                            .delay(_currentTransfer._cs_setup_time)
                            //TODO: What tolerance is perimissible here?
                            .tolerance(0);
                    dataPosted = true;
                }

            }
        }
        /* If sendData() was not posted: */
        if (!dataPosted) {
            /* Send data now */
            sendData();
        }

    }
    void sendData()
    {

    }
    void SPIirq(void *)
    {

    }
    void postXfer()
    {

    }

protected:
    PinName _currentSSEL;
    bool _SSELManaged;
    bool _SSELActiveHigh;
    bool _transferFirstXfer;
    //TODO: It's not possible to reinitialize a DigitalOut object.
    gpio_t _ssel;
protected:
    spi_t _spi;
    CThunk<SPI> _irq;
    SPI_xfer_data _currentTransfer;
    SPI_xfer_Queue_t _queue;
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



} // namespace mbed

#endif
