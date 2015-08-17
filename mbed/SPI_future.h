/* mbed Microcontroller Library
 * Copyright (c) 2006-2015 ARM Limited
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
#ifndef MBED_SPI_H
#define MBED_SPI_H

#include "platform.h"

#if DEVICE_SPI

#include "spi_api.h"

#if DEVICE_SPI_ASYNCH
#include "CThunk.h"
#include "dma_api.h"
#include "CircularBuffer.h"
#include "FunctionPointer.h"
#include "Transaction.h"
#endif


/**
 * SPI Transaction Details
 *      ___                 _                ____
 * SSEL    |_______________|_|______________|
 * SCK  __________XXXXXXXX______XXXXXXXX_________
 *         |     |       | | | |       |    |
 *         |     |       | | | |       |    |
 *         |     |       | | | |       |    + afterXfer()
 *         |     |       | | | |       + SPIirq()
 *         |     |       | | | + sendData()
 *         |     |       | | + beforeXfer()
 *         |     |       | + afterXfer()
 *         |     |       + SPIirq()
 *         |     + sendData()
 *         + beforeXfer()
 *
 * beforeXfer() :
 *     If a transfer is not active:
 *         Dequeue a transaction
 *         If there is a user pre-xfer callback:
 *             call the user pre-xfer callback
 *         Configure the SPI peripheral with the transaction's configuration parameters
 *         If the transaction requires autotoggle and the peripheral does not support it:
 *             Disable FIFOs
 *             Enable interrupt on every word
 *     If CS is not managed:
 *         If CS is not NC:
 *             set GPIO active (CS)
 *         If the setup delay is not 0:
 *             post sendData() to execute (setup delay) from now
 *     If sendData() was not posted:
 *         call sendData()
 *     Done
 *
 * sendData():
 *     If DMA is enabled:
 *         Start the DMA transfer
 *     Otherwise:
 *         If FIFOs are enabled:
 *             Fill the send FIFO
 *         Otherwise:
 *             call SendWord()
 *     Done
 *
 *
 * SPIirq():
 *     clear callSetCSInactive
 *     if FIFOs are enabled:
 *         empty the recv FIFO
 *         refill the send FIFO
 *     Otherwise:
 *         If CS is not managed:
 *             If the hold delay is not 0:
 *                 post callAfterXfer() to execute (hold delay) from now
 *         If callAfterXfer() was not posted:
 *             set callAfterXfer
 *     If there are no more words to send or receive:
 *         If there is a user post-xfer irqCallback:
 *             Call the user post-xfer irqCallback
 *         set callAfterXfer
 *     If callAfterXfer:
 *         call afterXfer()
 *     Done
 *
 * afterXfer():
 *     If CS is not managed:
 *         If CS is not NC:
 *             set GPIO inactive (CS)
 *         If inactive delay is not 0:
 *             post beforeXfer() to execute (inactive delay) from now
 *     If beforeXfer was not posted:
 *         call beforeXfer()
 *     Done
 */



namespace mbed {

class SPI_xfer_data {
    public:
        SPI_xfer_data(const SPI_xdata & xd);
        SPI_xfer_data();
        SPI_xfer_data & frequency(uint32_t freq);
        SPI_xfer_data & irq_callback(const event_callback_t &irqCallback);
        SPI_xfer_data & callback(const event_callback_t &callback);
        SPI_xfer_data & tx_buffer(const void * buf, size_t length);
        SPI_xfer_data & rx_buffer(void * buf, size_t length);
        SPI_xfer_data & event_mask(int mask);
        SPI_xfer_data & cs_pin(PinName cs);
        SPI_xfer_data & cs_setup_time(uint32_t microseconds);
        SPI_xfer_data & cs_hold_time(uint32_t microseconds);
        SPI_xfer_data & cs_inactive_time(uint32_t microseconds);
        SPI_xfer_data & mode(uint8_t bits, uint8_t mode, spi_bitorder_t order);
        SPI_xfer_data & dma_usage(DMAUsage usage);
        operator=(const SPI_xdata & xd);
        ~SPI_xfer_data();
    protected:
        uint32_t _freq;
        event_callback_t _irqCallback;
        event_callback_t _callback;
        void * _tx;
        void * _rx;
        size_t _tlen;
        size_t _rlen;
        int _eventMask;
        PinName _cs;
        uint32_t _cs_setup_time;
        uint32_t _cs_hold_time;
        uint32_t _cs_inactive_time;
        uint8_t _bits;
        uint8_t _spi_mode;
        spi_bitorder_t _order;
        DMAUsage _usage;
};

class SPI_interface {
    /** Start non-blocking SPI transfer.
     * @param tp The transfer Parameters descriptor object
     * @return Zero if the transfer has started, or an error code if
     *         a problem occurred.
     */
    virtual int post_transfer(const SPI_xfer_data & tp)=0;
    /** Abort the on-going SPI transfer, and continue with transfer's in the queue if any.
     */
    virtual void abort_transfer()=0;

    /** Clear the transaction buffer
     */
    virtual void clear_transfer_buffer()=0;

    /** Clear the transaction buffer and abort on-going transfer.
     */
    virtual void abort_all_transfers()=0;
}



class SPI : public SPI_interface {
public:
    /* APIs are accessors for an instance pointer */
    SPI(PinName mosi, PinName miso, PinName sclk, PinName cs = NC);
    /** Configure the data transmission format
     *
     *  @param bits  Number of bits per SPI frame (4 - 16)
     *  @param mode  Clock polarity and phase mode (0 - 3)
     *  @param order Bit order. SPI_MSB (standard) or SPI_LSB.
     *
     * @code
     * mode | POL PHA
     * -----+--------
     *   0  |  0   0
     *   1  |  0   1
     *   2  |  1   0
     *   3  |  1   1
     * @endcode
     */
    void format(int bits, int mode = 0, spi_bitorder_t order = SPI_MSB);

    /** Set the spi bus clock frequency
     *
     *  @param hz SCLK frequency in hz (default = 1MHz)
     */
    void frequency(int hz);

    SPI_xfer_data xd();

    ~SPI();

protected:
    static SPI_interface * get_instance(PinName mosi, PinName miso, PinName sclk);

protected:
    SPI_interface *_instance;
    SPI_xfer_data _xd;
};
} // namespace mbed

#endif

#endif
