/* mbed Microcontroller Library
 * Copyright (c) 2015-2016, ARM Limited, All Rights Reserved
 * SPDX-License-Identifier: Apache-2.0
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
#ifndef MBED_DRIVERS_V1_I2C_HPP
#define MBED_DRIVERS_V1_I2C_HPP

#include "mbed-drivers/platform.h"

#if DEVICE_I2C && DEVICE_I2C_ASYNCH


#include "mbed-hal/i2c_api.h"
#include "mbed-hal/dma_api.h"

#include "mbed-drivers/CThunk.h"
#include "core-util/FunctionPointer.h"
#include "core-util/PoolAllocator.h"

// Forward declarations
namespace mbed {
namespace drivers {
namespace v2 {
enum class I2CError;
} // namespace mbed
} // namespace drivers
} // namespace v2

#include "I2CDetail.hpp"
#include "EphemeralBuffer.hpp"

/// There are 4 possible I2C Events, so limit the I2C transaction handlers to 4
const size_t I2C_TRANSACTION_NHANDLERS = 4;

/**
 * \file
 * \brief A generic interface for I2C peripherals
 *
 * The I2C class interfaces with an I2C Resource manager in order to initiate Transactions and receive events. The
 * I2CTransaction class encapsulates all I2C transaction parameters. The I2CResourceManager class is a generic interface
 * for implementing I2C resource managers. This will allow for additional classes of I2C device, for example, a
 * bitbanged I2C master.
 *
 * # I2C
 * I2C encapsulates an I2C master. The physical I2C master to use is selected via the pins provided to the constructor.
 * The ```frequency()``` API sets the default frequency for transactions issued from the I2C object. This is used for
 * each transaction issued by I2C unless overridden when creating the transaction. Transactions are initiated by calling
 * ```transfer_to()``` or ```transfer_to_irqsafe()```. Both of these APIs create an instance of the ```TransferAdder```
 * helper class.
 *
 * # TransferAdder
 * The TransferAdder class allows convenient construction of complex transfers.
 *
 * The ```frequency()``` member overrides the default frequency set by the issuing I2C object.
 *
 * The ```on()``` member allows setting up to 4 event handlers, each with a corresponding event mask.
 *
 * The ```tx()``` members add a buffer to send to the transfer.
 *
 * The ```rx()``` members add a buffer to receive into to the transfer. There is a special case of ```rx()```, which
 * doesn't use a normal buffer. When ```rx(size_t)``` is called with a size of less than 8 bytes, the underlying
 * EphermeralBuffer is placed in ephemeral mode. This means that no preallocated receive buffer is needed, instead the
 * data is packed directly into the EphemeralBuffer. This has a side-effect that the data will be freed once the last
 * event handler has exited, so if the data must be retained, it should be copied out.
 *
 * The ```apply()``` method validates the transfer and adds it to the transaction queue of the I2CResourceManager. It
 * returns the result of validation.
 *
 * # I2C Resource Managers
 * I2C Resource managers are instantiated statically and initialized during global init. There is one Resource Manager
 * per logical I2C master. Logical I2C masters could consist of:
 *
 * * Onchip I2C masters
 * * I2C Bridges (SPI->I2C bridge, I2C->I2C bridge, etc.)
 * * Bit banged I2C
 * * Bit banged I2C over SPI GPIO expander
 * * More...
 *
 * Currently only onchip I2C masters are supported.
 *
 * # I2C transactions
 * An I2CTransaction contains a list of event handlers and their event masks, an I2C address, an operating frequency,
 * and zero or more I2CSegments. Zero-segment Transactions are explicitly supported since they are useful in connected
 * device discovery (pings).
 *
 * # I2C Segments
 * An I2CSegment is a wrapper around an EphemeralBuffer. It provides an I2C transfer direction (read or write) and an
 * optional callback to execute in IRQ context. I2CSegments also provide a chaining pointer so that they can perform
 * sequential or scatter/gather operations.
 *
 * # Constructing I2C transactions
 *
 * ```C++
 * void doneCB(bool dir, I2CTransaction *t, uint32_t event) {
 *     // Do something
 * }
 * I2C i2c0(sda, scl);
 * void app_start (int, char **) {
 *     uint8_t cmd[2] = {0xaa, 0x55};
 *     i2c0.transfer_to(addr).tx(cmd,2).rx(4).on(I2C_EVENT_ALL, doneCB);
 * }
 * ```
 */
namespace mbed {
namespace drivers {
namespace v2 {
// Forward declaration of I2C
class I2C;

/**
 * @brief List of error codes that can be produced by the I2C API
 */
enum class I2CError {
    None,
    InvalidMaster,
    PinMismatch,
    Busy,
    NullTransaction,
    NullSegment,
    MissingPoolAllocator,
    InvalidAddress,
    BufferSize,
    ScatterGatherNotSupported,
    DeinitInProgress
};

/**
 * A Transaction container for I2C
 */
class I2CTransaction {
public:
    /** I2C transfer callback
     *  @param The transaction that was running when the callback was triggered
     *  @param the event that triggered the calback
     */
    using event_callback_t = detail::I2C_event_callback_t;

    /**
     * Construct an I2C transaction and set the destination address at the same time
     *
     * @param[in] address set the I2C destination address
     */
    I2CTransaction(uint16_t address, uint32_t hz, bool irqsafe, I2C *issuer);
    ~I2CTransaction();

    /**
     * Get a new segment to be used by the transaction.
     * This API calls the associated I2C object's associated allocator.
     * @return a new I2CSegment
     */
    detail::I2CSegment * new_segment();

    /**
     * Install a new event handler with the corresponding event mask
     * Once all available event slots are consumed, further calls to add_event are ignored.
     *
     * @param[in] event The event mask on which to trigger cb
     * @param[in] cb The event to trigger when one or more bits in the event mask is matched
     * @retval false There was no space for a new event handler
     * @retval true The new event handler was installed
     */
    bool add_event(uint32_t event, const event_callback_t & cb);

    /**
     * The resource manager calls this API.
     * Calls the event handlers and
     */
    void process_event(uint32_t event);

    /**
     * Set the next transaction in the queue
     * This is an atomic operation that will append to the queue.
     */
    void append(I2CTransaction *t);

    /**
     * Forwards the irq-context callback to the segment
     * Also adds a pointer to this transaction to the callback
     * @param[in] the event that triggered this callback
     */
    void call_irq_cb(uint32_t event);

    /**
     * If the current segment is valid advance the segment pointer
     *
     * @retval true if the current segment is valid after this operation
     * @retval false if the current segment is not valid after this operation
     */
    bool advance_segment();

    /**
     * Reset the current segment to the root segment
     */
    void reset_current()
    {
        _current  = _root;
    }

    /**
     * Accessor for the next pointer
     * @return the next transaction
     */
    I2CTransaction * get_next()
    {
        return _next;
    }

    /**
     * Accessor for the Transactions's issuer
     * @return the I2C object that issued this transaction
     */
    I2C * get_issuer()
    {
        return _issuer;
    }

    /**
     * Accessor for the current segment pointer
     * @return the current segment poitner
     */
    detail::I2CSegment * get_current()
    {
        return _current;
    }

    /**
     * Accessor for the irqsafe flag
     * @retval true if the transaction was instantiated with irqsafe set and access to irq-safe allocators
     * @retval false if the transaction was not instantiated with irqsafe set and access to irq-safe allocators
     */
    bool is_irqsafe() const
    {
        return _irqsafe;
    }

    /**
     * Accessor for the transaction frequency
     * @return the frequency of the transaction in Hz
     */
    uint32_t frequency() const
    {
        return _hz;
    }

    /**
     * Accessor for the transaction frequency
     * @param[in] hz the transaction frequency in Hz
     */
    void frequency(uint32_t hz)
    {
        _hz = hz;
    }

    /**
     * Accessor for the transaction target address
     * @return the transaction address
     */
    uint16_t address() const
    {
        return _address;
    }

protected:
    /**
     * The next transaction in the queue
     * This field is used for chaining transactions together into a queue.
     *
     * This field is not volatile because it is only accessed from within a
     * critical section.
     */
    I2CTransaction * _next;
    /// The target I2C address to communicate with
    uint16_t _address;
    /**
     * The first I2CSegment in the transaction
     *
     * This field is not volatile because it is only accessed from within a
     * critical section. This will still be valid if the critical section is
     * replaced with an atomic operation.
     */
    detail::I2CSegment * _root;
    /**
     * The first I2CSegment in the transaction
     *
     * This is a helper field for building or processing I2C transactions.
     * It allows the Transaction to easily locate the end of the queue when
     * composing the transaction and, equally, the currently transferring segment
     * while processing the transaction.
     *
     * This field is not volatile because it is only accessed from within a
     * critical section.
     */
    detail::I2CSegment * _current;
    /// The I2C frequency to use for the transaction
    unsigned _hz;
    /// Flag to indicate that the Transaction and its Segments were allocated with an irqsafe allocator
    bool _irqsafe;
    /// The I2C Object that launched this transaction
    I2C * _issuer;
    /// An array of I2C Event Handlers.
    detail::I2CEventHandler _handlers[I2C_TRANSACTION_NHANDLERS];
};

/** An I2C Master, used for communicating with I2C slave devices
 *
 * Example:
 * @code
 * // Read 6 bytes from I2C EEPROM slave at address 0x62
 *
 * #include "mbed.h"
 *
 * mbed::drivers::v2::I2C i2c(p28, p27);
 *
 * // This callback executes in minar context
 * void xfer_done(mbed::drivers::v2::I2CTransaction * t, int event) {
 *     t->reset_current(); // Set the current pointer to root.
 *     uint8_t *txPtr = t->get_current->get_buf();
 *     printf("EEPROM 0x%02x@0x%02x%02x: ", t->address(), txPtr[0], txPtr[1]);
 *     // Get the rx buffer pointer
 *     uint8_t * rxPtr = t->get_current() // get the first segment (the tx segment)
 *                        ->get_next()    // get the second segment (the rx segment)
 *                        ->get_buf();    // get the buffer pointer
 *     for (uint i = 0; i < 6; i++) {
 *         printf("%02x", rxPtr[i]);
 *     }
 *     printf("\n");
 *     // Both the tx and rx buffers are ephemeral, so they will be freed automatically when this function exits
 * }
 *
 * void app_start(int, char **) {
 *     i2c.transfer_to(0x62)            // I2C Slave Address
 *         .tx_ephemeral("\x12\x34", 2) // Send EEPROM location
 *         .rx(6)                       // Read 6 bytes into an ephemeral buffer
 *         .on(I2C_EVENT_TRANSFER_COMPLETE, xfer_done)
 *         .apply();
 * }
 * @endcode
 */
class I2C {
public:
    using event_callback_t = detail::I2C_event_callback_t;

    /** Create an I2C Master interface, connected to the specified pins
     *
     *  @param sda I2C data line pin
     *  @param scl I2C clock line pin
     */
    I2C(PinName sda, PinName scl);

    /** Create an I2C Master interface, connected to the specified pins and providing IRQ-safe allocators
     *
     *  @param sda I2C data line pin
     *  @param scl I2C clock line pin
     *  @param TransactionPool An IRQ-safe allocator for Transaction objects
     *  @param SegmentPool An IRQ-safe allocator for Segment objects
     */
    I2C(PinName sda, PinName scl, mbed::util::PoolAllocator *TransactionPool, mbed::util::PoolAllocator *SegmentPool);

    /** Destroy the I2C Master interface.
     *  Releases a reference to the I2C Resource Manager
     */
    ~I2C();

    /** Set the frequency of the I2C interface
     *
     *  @param hz The bus frequency in hertz
     */
    void frequency(uint32_t hz);

    /**
     * @brief A helper class for constructing transactions
     */
    class TransferAdder {
        friend I2C;
    protected:
        /**
         * @brief Construct a new TransferAdder
         *
         * @param[in] i2c the issuing I2C object
         * @param[in] address the address that is the target of the transfer
         * @param[in] hz the frequency to use for the transfer
         * @param[in] irqsafe indicates whether the TransferAdder should use the I2C Object's IRQ-safe allocators
         */
        TransferAdder(I2C *i2c, int address, uint32_t hz, bool irqsafe);

        /**
         * @brief Allocates and constructs a new I2C Segment
         * @return A new I2C Segment
         */
        detail::I2CSegment * new_segment(detail::I2CDirection d);

    public:
        /**
         * @brief Set the frequency for this transaction
         *
         * By default, the transaction will use the default frequency for the I2C object. This overrides that frequency.
         * The frequency used will be applied to the whole transaction, not just a single segment.
         *
         * @param[in] hz the frequency to set
         */
        TransferAdder & frequency(uint32_t hz);

        /**
         * @brief set an event handler
         *
         * An event is triggered when any of the bits in the event mask match the event.
         * Four event slots are provided, additional event
         *
         * @param[in] event the event mask
         * @param[in] cb the callback to trigger on an event mask match
         */
        TransferAdder & on(uint32_t event, const event_callback_t & cb);

        /**
         * @brief set an event handler
         *
         * An event is triggered when any of the bits in the event mask match the event.
         * Four event slots are provided, additional event
         *
         * @param[in] event the event mask
         * @param[in] cb the callback to trigger on an event mask match
         */
        TransferAdder & on(uint32_t event, event_callback_t && cb);

        /**
         * @brief Queue the transfer
         *
         * Hands the transfer over to the resource manager and returns the resource manager's status. No further
         * configuration of the transfer is possible after apply() has been called.
         *
         * @return the error status of submitting the transfer to the resource manager
         */
        I2CError apply();

        /**
         * @brief Add a transmit buffer to the transaction
         *
         * @param[in] buf a pointer to the buffer to send
         * @param[in] len the number of bytes to send
         */
        TransferAdder & tx(void *buf, size_t len);

        /**
         * @brief Add a transmit buffer to the transaction
         *
         * @param[in] buf a pointer to and length of the buffer to send
         */
        TransferAdder & tx(const Buffer & buf);

        /**
         * @brief Add an ephermeral transmit buffer to the transaction
         *
         * If the buffer is 7 or fewer bytes, it will be managed internally, so the original can be freed.
         *
         * @param[in] buf a pointer to the buffer to send
         * @param[in] len the number of bytes to send
         */
        TransferAdder & tx_ephemeral(void *buf, size_t len);

        /**
         * @brief Add a receive buffer to the transaction
         *
         * @param[in] buf a pointer to the buffer to receive into
         * @param[in] len the number of bytes to receive
         */
        TransferAdder & rx(void *buf, size_t len);

        /**
         * @brief Add a receive buffer to the transaction
         *
         * @param[in] buf a pointer to and length of the buffer to receive into
         */
        TransferAdder & rx(const Buffer & buf);

        /**
         * @brief Add an ephermeral receive buffer to the transaction
         *
         * If the buffer is 7 or fewer bytes, it will be managed internally
         *
         * @param[in] len the number of bytes to receive
         */
        TransferAdder & rx(size_t len);

        /**
         * @brief Applies an unapplied transaction or destroys a failed transaction
         *
         * If the transaction has not been applied when the TransferAdder goes out of scope, it is automatically
         * applied. If it has already been applied, the destructor exits. If an error condition has been detected, the
         * destructor destroys the transaction and any associated segments.
         */
        ~TransferAdder();
    protected:
        /// The transaction object that is to be added to the I2C transaction queue
        I2CTransaction * _xact;
        /// The I2C object to use for posting the transaction
        I2C* _i2c;
        /// flag variable to prevent double-posting of transactions
        bool _posted;
        /// flag variable to indicate whether the transaction is intended to use irq-safe allocators
        bool _irqsafe;
        /// The error status of the TransferAdder. Transaction will only be posted if the error status is I2CError::none
        I2CError _rc;
    };

    /**
     * @brief Begin constructing a transfer to the specified I2C address
     *
     * Creates a TransferAdder to manage the construction of the transfer. This API should not be called from IRQ
     * context
     *
     * @param[in] address the I2C address that is the target of this transaction
     */
    TransferAdder transfer_to(int address);

    /**
     * @brief Begin constructing a transfer to the specified I2C address, in irq context
     *
     * Creates a TransferAdder to manage the construction of the transfer. This API can be called from IRQ context, but
     * It requires that a pool allocators for both Transactions and Segments have been specified.
     *
     * @param[in] address the I2C address that is the target of this transaction
     */
    TransferAdder transfer_to_irqsafe(int address);

    /**
     * @brief Create a new segment
     *
     * If irqsafe = true, allocate from a pool allocator. Otherwise, allocate from new.
     *
     * @param[in] irqsafe flag that indicates whether or not to use a pool allocator
     * @return the new segment on success, or NULL on failure
     */
    detail::I2CSegment * new_segment(bool irqsafe);

    /**
     * @brief Free a transaction
     *
     * Determines destroys and frees a transaction. If the transaction was marked irqsafe, calls the destructor then the
     * pool allocator's free member function. If the transaction was not marked irqsafe, calls delete.
     *
     * @param[in] t the transaction to destroy and free
     */
    void free(I2CTransaction *t);

    /**
     * @brief Free a segment
     *
     * Determines destroys and frees a segment. If irqsafe = true, calls the destructor then the
     * pool allocator's free member function. If irqsafe = false, calls delete.
     *
     * @param[in] s the segment to destroy and free
     * @param[in] irqsafe a flag that indicates whether to use the pool allocator to free or not
     */
    void free(detail::I2CSegment *s, bool irqsafe);

protected:
    friend TransferAdder;

    /**
     * @brief Initiate a transaction
     *
     * Submits the transaction to the resource manager's queue and returns the result
     *
     * @param[in] t the transaction to queue
     * @return the status of the submission
     */
    I2CError post_transaction(I2CTransaction *t);

    /**
     * @brief Creates a new transaction and pre-fills some parts of it.
     *
     * new_transaction prefills the address, frequency, and issuer fields. If marked irqsafe, it will be allocated from
     * the pool allocator and any associated segments will be allocated from the pool allocator as well.
     *
     * @param[in] address The I2C address that is the target of this transaction
     * @param[in] hz The I2C frequency to use
     * @param[in] irqsafe The flag that indicates whether to use pool allocators
     * @param[in] issuer A pointer to this I2C instance
     * @return the new I2C Transaction object, or NULL on failure
     */
    I2CTransaction * new_transaction(uint16_t address, uint32_t hz, bool irqsafe, I2C *issuer);

    uint32_t _hz;
    detail::I2CResourceManager * _owner;
    mbed::util::PoolAllocator * TransactionPool;
    mbed::util::PoolAllocator * SegmentPool;
};
} // namespace v2
} // namespace drivers
} // namespace mbed

#endif

#endif // MBED_DRIVERS_V1_I2C_HPP
