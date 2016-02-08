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
#ifndef MBED_SPI_H
#define MBED_SPI_H

#include "platform.h"

#if DEVICE_SPI

#include "spi_api.h"

#if DEVICE_SPI_ASYNCH
#include "CThunk.h"
#include "dma_api.h"
#include "CircularBuffer.h"
#include "core-util/FunctionPointer.h"
#include "Transaction.h"

#ifndef YOTTA_CFG_MBED_DRIVERS_SPI_TRANSACTION_QUEUE
#   define YOTTA_CFG_MBED_DRIVERS_SPI_TRANSACTION_QUEUE 16
#endif
// backwards compatible guard for definition in `mbed-hal-<chip>/target_config.h`
#ifndef TRANSACTION_QUEUE_SIZE_SPI
#   define TRANSACTION_QUEUE_SIZE_SPI     YOTTA_CFG_MBED_DRIVERS_SPI_TRANSACTION_QUEUE
#endif

#endif

namespace mbed {

/** A SPI Master, used for communicating with SPI slave devices
 *
 * The default format is set to 8-bits, mode 0, and a clock frequency of 1MHz
 *
 * NOTE: This information will be deprecated soon.
 * Most SPI devices will also require Chip Select and Reset signals. These
 * can be controlled using <DigitalOut> pins
 */
class SPI {

#if DEVICE_SPI_ASYNCH
public:
    /** SPI transfer callback
     *  @param Buffer the tx buffer
     *  @param Buffer the rx buffer
     *  @param int the event that triggered the calback
     */
    typedef mbed::util::FunctionPointer3<void, Buffer, Buffer, int> event_callback_t;
private:
    typedef TwoWayTransaction<event_callback_t> transaction_data_t;
    typedef Transaction<SPI, transaction_data_t> transaction_t;
#endif
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
    class SPITransferAdder {
        friend SPI;
    private:
        SPITransferAdder(SPI *owner);
        const SPITransferAdder & operator =(const SPITransferAdder &a);
        SPITransferAdder(const SPITransferAdder &a);
    public:
        /** Set the transmit buffer
         *  Sets the transmit buffer pointer and transmit size.
         *
         *  NOTE: Repeated calls to tx() override buffer parameters.
         *
         *  @param[in] txBuf a pointer to the transmit buffer
         *  @param[in] txSize the size of the transmit buffer
         *  @return a reference to the SPITransferAdder
     */
        SPITransferAdder & tx(void *txBuf, size_t txSize);
        /** Set the receive buffer
         *  Sets the receive buffer pointer and receive size
     *
         *  NOTE: Repeated calls to rx() override buffer parameters.
         *
         *  @param[in] rxBuf a pointer to the receive buffer
         *  @param[in] rxSize the size of the receive buffer
         *  @return a reference to the SPITransferAdder
     */
        SPITransferAdder & rx(void *rxBuf, size_t rxSize);
        /** Set the SPI Event callback
         *  Sets the callback to invoke when an event occurs and the mask of
         *  which events should trigger it. The callback will be scheduled to
         *  execute in main context, not invoked in interrupt context.
     *
         *  NOTE: Repeated calls to callback() override callback parameters.
         *
         *  @param[in] cb The event callback function
         *  @param[in] event     The logical OR of SPI events to modify. Look at spi hal header file for SPI events.
         *  @return a reference to the SPITransferAdder
         */
        SPITransferAdder & callback(const event_callback_t &cb, int event);
        /** Initiate the transfer
         *  apply() allows the user to explicitly activate the transfer and obtain
         *  the return code from the validation of the transfer parameters.
         * @return Zero if the transfer has started, or -1 if SPI peripheral is busy
         */
        int apply();
        ~SPITransferAdder();
    private:
        transaction_data_t _td;
        bool _applied;
        int _rc;
        SPI * _owner;
    };
    /** Start an SPI transfer
     *  The transfer() method returns a SPITransferAdder.  This class allows each
     *  parameter to be set with a dedicated method.  This way, the many optional
     *  parameters are easy to identify and set.
     *
     *
     *  @return A SPITransferAdder object.  When either apply() is called or the
     *      SPITransferAdder goes out of scope, the transfer is queued.
     */
    SPITransferAdder transfer();

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

    /** Add a transfer to the queue
     * @param data Transaction data
     * @return Zero if a transfer was added to the queue, or -1 if the queue is full
    */
    int queue_transfer(const transaction_data_t &td);

    /** Configures a callback, spi peripheral and initiate a new transfer
     *
     * @param data Transaction data
    */
    void start_transfer(const transaction_data_t &td);

    /** Start a new transaction
     *
     *  @param data Transaction data
    */
    void start_transaction(transaction_data_t *data);

    /** Dequeue a transaction
     *
    */
    void dequeue_transaction();

    /** Initiate a transfer
     * @param xfer the SPITransferAdder object used to create the SPI transfer
     * @return the result of validating the transfer parameters
     */
    int transfer(const SPITransferAdder &xfer);
#endif

public:
    virtual ~SPI() {
    }

protected:
    spi_t _spi;

#if DEVICE_SPI_ASYNCH
#if TRANSACTION_QUEUE_SIZE_SPI
    static CircularBuffer<transaction_t, TRANSACTION_QUEUE_SIZE_SPI> _transaction_buffer;
#endif
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
    bool _busy;
};

} // namespace mbed

#endif

#endif
