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
#include "mbed-util/FunctionPointer.h"
#include "Transaction.h"
#endif

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
    typedef mbed::util::FunctionPointer3<void, Buffer, Buffer, int> event_callback_t;

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
