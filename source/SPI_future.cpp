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
        _irq(this), _useDMA(false)
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
        if (!_active) {
            _active = true;
            preXfer();
        }
    }
protected:
    void _startTransfer()
    {
        /* If there is a user pre-xfer callback: */
        if (_xfer._irqPreCallback) {
            /* call the user pre-xfer callback */
            _xfer._irqPreCallback(&_xfer);
        }
        spi_format(&_spi,
            _xfer._bits,
            _xfer._mode,
            _xfer._order,
            0);
        spi_frequency(&_spi, _xfer._hz);
        _wordSize = _xfer._bits / CHAR_BITS;
        _tx_offset = 0;
        _rx_offset = 0;
        while (_wordSize & (_wordSize - 1)) {
            _wordSize++;
        }
        if (_xfer._cs != NC) {
            /* If CS can be managed by the SPI peripheral */
            _SSELManaged = (pinmap_peripheral(PinMap_SPI_SSEL, _xfer._cs) != NC);
            if (_SSELManaged) {
                /* Configure the CS pin */
                pinmap_pinout(_xfer._cs, PinMap_SPI_SSEL);
            } else {
                /* Configure the CS pin as GPOut */
                gpio_init_out(&_ssel, _xfer._cs);
                /* set GPIO active (CS) */
                gpio_write(&_ssel, _xfer._csActiveHigh);
            }
            // TODO:

            ///* If the transaction requires autotoggle and the peripheral does not support it:
            //if (spi_can_toggle(&_spi, _xfer._cs)) {
            //    /* Disable FIFOs */
            //    spi_fifo_set(&_spi, false);
            //    _useFIFO = false;
            //    /* Enable interrupt on every word */
            //    spi_irq_set(&_spi, ???);
            //}
        }
    }
    /* SPI State machine functions */
    void preXfer()
    {
        bool dataPosted = false;
        /* If a transfer is not active: */
        if (!_active || xfer_done())
        {
            if (_queue.empty()){
                // TODO: Halt the SPI peripheral, clock gate, power gate
                _active = false;
                spi_power_down(&_spi);
                return;
            }
            if (!active) {
                spi_power_up(&_spi);
            }
            /* Dequeue a transaction */
            _queue.pop(_xfer);
            _startTransfer();
            if (!_SSELManaged && _xfer._cs_setup_time != 0) {
                minar::Scheduler::postCallback(event_callback_t(this, &SPI_physical::sendData).bind())
                        .delay(_xfer._cs_setup_time)
                        .tolerance(0);
                dataPosted = true;
            }
        } else {
            /* If CS is not managed: */
            if (!_SSELManaged && _xfer._csAutoToggle) {
                /* If CS is not NC: */
                if (_xfer._cs != NC) {
                    /* set GPIO active (CS) */
                    gpio_write(&_ssel, _xfer._csActiveHigh);
                }
                /* If the setup delay is not 0: */
                if (_xfer._cs_setup_time != 0) {
                    /* post sendData() to execute (setup delay) from now */
                    minar::Scheduler::postCallback(event_callback_t(this, &SPI_physical::sendData).bind())
                            .delay(_xfer._cs_setup_time)
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
    uint32_t get_next_tx_value() {
        uint32_t value;
        if (_tx_offset >= _xfer._tlen) {
            value = _xfer._tx_fill;
        } else {
            uintptr_t ptr = (uintptr_t)_xfer._tx + _tx_offset * _wordSize;
            switch(_wordSize) {
                case 1:
                    value = *(uint8_t *)ptr;
                    break;
                case 2:
                    value = *(uint16_t *)ptr;
                    break;
                case 4:
                    value = *(uint32_t *)ptr;
                    break;
                default:
                    MBED_ASSERT("invalid _bits for SPI");
                    break;
            }
        }
        return value;
    }
    bool xfer_done() {
        return ((_tx_offset >= _xfer._tlen) && (_rx_offset >= _xfer._rlen));
    }
    void spi_fill_fifo()
    {
        while (!spi_tx_fifo_full(&_spi) && !xfer_done()) {
            spi_send_word(&_spi,get_next_tx_value());
            _tx_offset++;
        }
    }
    void sendData()
    {
        /* If DMA is enabled: */
        if (_useDMA) {
            /* Start the DMA transfer */
            // TODO: DMA
            // spi_start_dma(&_spi, _xfer._tx, _xfer._tlen, _xfer._rx, _xfer._rlen)
        }
        /* If FIFOs are enabled: */
        else if (_useFIFO) {
            /* Fill the send FIFO */
            spi_fill_fifo();
        } else {
            /* Send one word */
            spi_send_word(&_spi,get_next_tx_value());
            _tx_offset++;
        }
    }
    void store_word(uint32_t value)
    {
        if (_rx_offset >= _xfer._rlen) {
        } else {
            uintptr_t ptr = (uintptr_t)_xfer._rx + _rx_offset * _wordSize;
            switch(_wordSize) {
                case 1:
                    *(uint8_t *)ptr = value;
                    break;
                case 2:
                    *(uint16_t *)ptr = value;
                    break;
                case 4:
                    *(uint32_t *)ptr = value;
                    break;
                default:
                    MBED_ASSERT("invalid _bits for SPI");
                    break;
            }
        }
    }
    void spi_drain_fifo()
    {
        while (!spi_rx_fifo_empty()) {
            uint32_t value = spi_get_word(&_spi);
            store_word(value);
        }
    }
    void SPIirq()
    {
        bool callSetCSInactive = false;

        if (_useDMA) {
            //TODO:
        }
        else if (_useFIFO) {
            spi_fifo_drain();
            if (!xfer_done()) {
                spi_fifo_fill();
            }
        } else {
            store_word(spi_get_word(&_spi));
            _rx_offset++;
            if (!xfer_done()) {
                spi_send_word(&_spi,get_next_tx_value());
                _tx_offset++;
            }
        }
        if (!_SSELManaged && _xfer._cs_hold_time && (_xfer._csAutoToggle || xfer_done())) {
            minar::Scheduler::postCallback(event_callback_t(this, &SPI_physical::postXfer).bind())
                    .delay(_xfer._cs_hold_time)
                    .tolerance(0);
        } else {
            callSetCSInactive = true;
        }
        if (xfer_done() && _xfer._irqPostCallback {
                _xfer._irqPostCallback();
            }
        }
        if (callSetCSInactive) {
            postXfer();
        }

    }
    void postXfer()
    {
        bool callPreXfer = false;
        if (!_SSELManaged && _xfer._cs != NC) {
            gpio_write(&_ssel, !_xfer._csActiveHigh);
        }
        if (xfer_done() && _xfer._callback){
            minar::Scheduler::postCallback(_xfer._callback.bind());
        }
        if (!_SSELManaged && _xfer._cs_inactive_time  ) {
            minar::Scheduler::postCallback(event_callback_t(this, &SPI_physical::preXfer).bind())
                    .delay(_xfer._cs_inactive_time)
                    .tolerance(0);
        } else {
            preXfer();
        }
    }

protected:
    /* SSEL managment */
    bool _SSELManaged;
    //TODO: It's not possible to reinitialize a DigitalOut object.
    gpio_t _ssel;
protected:
    /* Transfer Management */
    bool _useDMA;
    bool _useFIFO;
    size_t _tx_offset;
    size_t _rx_offset;
    size_t _tr_total;
    uint32_t _tx_fill;
    uint8_t _wordSize;

protected:
    spi_t _spi;
    CThunk<SPI> _irq;
    SPI_xfer_data _xfer;
    SPI_xfer_Queue_t _queue;
    volatile bool _active;
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

void SPI::set_dma_usage(DMAUsage usage)
{
    _xd.dma_usage(usage);
}

SPI_xfer_data SPI::xd() {
    return _xd;
}

SPI_xfer_data::SPI_xfer_data():
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
SPI_xfer_data & SPI_xfer_data::frequency(uint32_t freq)
{
    _freq = freq;
    return *this;
}
SPI_xfer_data & SPI_xfer_data::irq_callback(event_callback_t irqCallback)
{
    _irqCallback = irqCallback;
    return *this;
}
SPI_xfer_data & SPI_xfer_data::callback(event_callback_t callback)
{
    _callback = callback;
    return *this;
}
SPI_xfer_data & SPI_xfer_data::tx_buffer(void * buf, size_t length)
{
    _tx = buf;
    _tlen = length;
    return *this;
}
SPI_xfer_data & SPI_xfer_data::rx_buffer(void * buf, size_t length)
{
    _rx = buf;
    _rlen = length;
    return *this;
}
SPI_xfer_data & SPI_xfer_data::event_mask(int mask)
{
    _eventMask = mask;
    return *this;
}
SPI_xfer_data & SPI_xfer_data::cs_pin(PinName cs)
{
    _cs = cs;
    return *this;
}

SPI_xfer_data::~SPI_xfer_data() {
    if(_spi && !_posted) {
        int err = _spi->transfer(_tx,_tlen,_rx,_rlen,_callback,_eventMask);
        if (err && _callback) {
            minar::Scheduler::postCallback(_callback.bind(SPI_EVENT_ERROR));
        }
    }
}

} // namespace mbed

#endif
