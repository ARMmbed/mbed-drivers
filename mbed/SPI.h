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
 *      ___                 _               ____
 * SSEL    |_______________|_|_____________|
 * SCK  __________XXXXXXXX_____XXXXXXXX_________
 *         |     |       |    |       |    |  + SSEL_clear_hold_timeout
 *         |     |       |    |       |    |  + Dequeue next transaction
 *         |     |       |    |       |    + post-transfer timer expires
 *         |     |       |    |       |    + clear_SSEL
 *         |     |       |    |       |    + postCallback() -> [USER]
 *         |     |       |    |       + word_done
 *         |     |       |    |       + xfer_done
 *         |     |       |    |       + irqPostCB() -> [USER]
 *         |     |       |    |       + start post-transfer timer
 *         |     |       |    + sw_auto_toggle_end (optional)
 *         |     |       |    + word_start
 *         |     |       + word_done
 *         |     |       + sw_auto_toggle_start (optional)
 *         |     + setup_timeout expires
 *         |     + xfer_start
 *         |     + word_start
 *         +
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
 * SPI irq():
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

/** A SPI Master, used for communicating with SPI slave devices
 *
 * The default format is set to 8-bits, mode 0, and a clock frequency of 1MHz
 *
 * Most SPI devices will also require Chip Select and Reset signals. These
 * can be controlled using <DigitalOut> pins
 *
 * Example:
 * @code
 * // Send a byte to a SPI slave, and record the response
 *
 * #include "mbed.h"
 *
 * SPI device(p5, p6, p7); // mosi, miso, sclk
 *
 * int main() {
 *     int response = device.write(0xFF);
 * }
 * @endcode
 */
class SPI {

public:
    /** Create a SPI master connected to the specified pins
     *
     * Pin Options:
     *  (5, 6, 7) or (11, 12, 13)
     *
     *  mosi or miso can be specfied as NC if not used
     *
     *  @param mosi SPI Master Out, Slave In pin
     *  @param miso SPI Master In, Slave Out pin
     *  @param sclk SPI Clock pin
     */
    SPI(PinName mosi, PinName miso, PinName sclk);

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
    void frequency(int hz = 1000000);

    /** Write to the SPI Slave and return the response
     *
     *  @param value Data to be sent to the SPI slave
     *
     *  @returns
     *    Response from the SPI slave
    */
    virtual int write(int value);

#if DEVICE_SPI_ASYNCH
    /** SPI transfer callback
     *  @param Buffer the tx buffer
     *  @param Buffer the rx buffer
     *  @param int the event that triggered the calback
     */
    typedef FunctionPointer3<void, Buffer, Buffer, int> event_callback_t;

    /** Start non-blocking SPI transfer.
     *
     * @param tx_buffer The TX buffer with data to be transfered. If NULL is passed,
     *                  the default SPI value is sent
     * @param tx_length The length of TX buffer
     * @param rx_buffer The RX buffer which is used for received data. If NULL is passed,
     *                  received data are ignored
     * @param rx_length The length of RX buffer
     * @param callback  The event callback function
     * @param event     The logical OR of SPI events to modify. Look at spi hal header file for SPI events.
     * @return Zero if the transfer has started, or -1 if SPI peripheral is busy
     */
    int transfer(void *tx_buffer, int tx_length, void *rx_buffer, int rx_length, const event_callback_t& callback, int event = SPI_EVENT_COMPLETE);
    class TransferParameters {
        friend SPI;
    public:
        TransferParameters();
        TransferParameters & frequency(uint32_t freq);
        TransferParameters & irq_callback(const event_callback_t &irqCallback);
        TransferParameters & callback(const event_callback_t &callback);
        TransferParameters & tx_buffer(const void * buf, size_t length);
        TransferParameters & rx_buffer(void * buf, size_t length);
        TransferParameters & event_mask(int mask);
        TransferParameters & cs_pin(PinName cs);
        TransferParameters & cs_setup_time(uint32_t microseconds);
        TransferParameters & cs_hold_time(uint32_t microseconds);

        ~TransferParameters();
    private:
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
        uint8_t _spi_mode;
    }
    int post_transfer(const TransferParameters & tp) {
        return transfer(tp._tx,tp._tlen,tp._rx,tp._rlen,tp._callback,tp._eventMask);
    }

     /** Start non-blocking SPI transfer.
     *
     * @param tx_buffer The TX buffer with data to be transfered. If NULL is passed,
     *                  the default SPI Avalue is sent
     * @param rx_buffer The RX buffer which is used for received data. If NULL is passed,
     *                  received data are ignored
     * @param callback  The event callback function
     * @param event     The logical OR of SPI events to modify. Look at spi hal header file for SPI events.
     * @return Zero if the transfer has started, or -1 if SPI peripheral is busy
     */
    int transfer(const Buffer& tx_buffer, const Buffer& rx_buffer, const event_callback_t& callback, int event = SPI_EVENT_COMPLETE);

    /** Abort the on-going SPI transfer, and continue with transfer's in the queue if any.
     */
    void abort_transfer();

    /** Clear the transaction buffer
     */
    void clear_transfer_buffer();

    /** Clear the transaction buffer and abort on-going transfer.
     */
    void abort_all_transfers();

    /** Configure DMA usage suggestion for non-blocking transfers
     *
     *  @param usage The usage DMA hint for peripheral
     *  @return Zero if the usage was set, -1 if a transaction is on-going
    */
    int set_dma_usage(DMAUsage usage);

protected:
    /** SPI IRQ handler
     *
    */
    void irq_handler_asynch(void);

   /**
     *
     * @param tx_buffer The TX buffer with data to be transfered. If NULL is passed,
     *                  the default SPI value is sent
     * @param tx_length The length of TX buffer in bytes
     * @param rx_buffer The RX buffer which is used for received data. If NULL is passed,
     *                  received data are ignored
     * @param rx_length The length of RX buffer in bytes
     * @param bit_width The buffers element width
     * @param callback  The event callback function
     * @param event     The logical OR of events to modify
     * @return Zero if a transfer was added to the queue, or -1 if the queue is full
    */
    int queue_transfer(const Buffer& tx, const Buffer& rx, const event_callback_t& callback, int event);

    /** Configures a callback, spi peripheral and initiate a new transfer
     *
     * @param tx_buffer The TX buffer with data to be transfered. If NULL is passed,
     *                  the default SPI value is sent
     * @param tx_length The length of TX buffer in bytes
     * @param rx_buffer The RX buffer which is used for received data. If NULL is passed,
     *                  received data are ignored
     * @param rx_length The length of RX buffer in bytes
     * @param bit_width The buffers element width
     * @param callback  The event callback function
     * @param event     The logical OR of events to modify
    */
    void start_transfer(const Buffer& tx, const Buffer& rx, const event_callback_t& callback, int event);

#if TRANSACTION_QUEUE_SIZE_SPI

    typedef TwoWayTransaction<event_callback_t> transaction_data_t;
    typedef Transaction<SPI, transaction_data_t> transaction_t;

    /** Start a new transaction
     *
     *  @param data Transaction data
    */
    void start_transaction(transaction_data_t *data);

    /** Dequeue a transaction
     *
    */
    void dequeue_transaction();
    static CircularBuffer<transaction_t, TRANSACTION_QUEUE_SIZE_SPI> _transaction_buffer;
#endif

#endif

public:
    virtual ~SPI() {
    }

protected:
    spi_t _spi;

#if DEVICE_SPI_ASYNCH
    CThunk<SPI> _irq;
    transaction_data_t _current_transaction;
    DMAUsage _usage;
#endif

    void aquire(void);
    static SPI *_owner;
    int _bits;
    int _mode;
    spi_bitorder_t _order;
    int _hz;
};

} // namespace mbed

#endif

#endif
