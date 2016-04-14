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

#include "mbed-drivers/platform.h"

#if DEVICE_I2C && DEVICE_I2C_ASYNCH

#include "mbed-drivers/v2/I2C.hpp"
#include "core-util/CriticalSectionLock.h"
#include "core-util/atomic_ops.h"
#include "core-util/assert.h"
#include "minar/minar.h"

namespace mbed {
namespace drivers {
namespace v2 {
namespace detail {

I2CError I2CResourceManager::post_transaction(I2CTransaction *t)
{
    CORE_UTIL_ASSERT(t != nullptr);
    if (!t) {
        return I2CError::NullTransaction;
    }
    I2CError rc = validate_transaction(t);
    if (rc != I2CError::None) {
        return rc;
    }

    // This can't be lock free because of the need to call append() on _TransactionQueue.
    mbed::util::CriticalSectionLock lock;
    I2CTransaction * tx = _TransactionQueue;

    if (tx) {
        tx->append(t);
    } else {
        _TransactionQueue = t;
        return start_transaction();
    }
    return I2CError::None;
}

void I2CResourceManager::process_event(uint32_t event)
{
    I2CTransaction * t;
    CORE_UTIL_ASSERT(_TransactionQueue != nullptr);
    if (!_TransactionQueue) {
        return;
    }
    // Get the current item in the transaction queue
    t = _TransactionQueue;
    if (!t) {
        return;
    }
    // If the event is 0, no further action is required
    if (!event) {
        return;
    }
    // Fire the irqcallback for the segment
    t->call_irq_cb(event);
    {
        // This isn't done with atomics due to the side-effects.
        mbed::util::CriticalSectionLock lock;

        // If there is another segment to process, advance the segment pointer
        // Record whether there was another segment
        bool TransactionDone = !t->advance_segment();
        // If there was an event that is not a complete event
        // or there was a complete event and the next segment is nullptr
        if ((event & I2C_EVENT_ALL & ~I2C_EVENT_TRANSFER_COMPLETE) ||
                ((event & I2C_EVENT_TRANSFER_COMPLETE) && TransactionDone)) {
            // fire the handler
            minar::Scheduler::postCallback(
                I2C_event_callback_t(this, &I2CResourceManager::handle_event).bind(t,event)
            );
            // Advance to the next transaction
            _TransactionQueue = t->get_next();
            if (_TransactionQueue) {
                // Initiate the next transaction
                start_transaction();
            } else {
            }
        } else if (!TransactionDone) {
            start_segment();
        }
    }
}

void I2CResourceManager::handle_event(I2CTransaction *t, uint32_t event)
{
    t->process_event(event);
    // This happens after the callbacks have all been called
    t->get_issuer()->free(t);
}

I2CResourceManager::I2CResourceManager() : _TransactionQueue(nullptr) {}

I2CResourceManager::~I2CResourceManager()
{
    mbed::util::CriticalSectionLock lock;
    while (_TransactionQueue) {
        I2CTransaction * tx = (_TransactionQueue);
        _TransactionQueue = tx->get_next();
        tx->get_issuer()->free(tx);
    }
}

class HWI2CResourceManager : public I2CResourceManager
{
public:
    HWI2CResourceManager(const size_t id, void(*handler)(void)):
        _scl(NC),
        _sda(NC),
        _i2c(),
        _id(id),
        _references(0),
        _handler(handler)
    {}

    virtual I2CError init(PinName sda, PinName scl)
    {
        /*
         * Calling init during a transaction could cause communication artifacts, so only a single call to init is
         * permitted unless all references are dropped
         */
        if (_references == 0 && (_scl != NC || _sda != NC)) {
            return I2CError::DeinitInProgress;
        }
        if (mbed::util::atomic_incr<std::uint32_t>(const_cast<std::uint32_t *>(&_references), 1) == 1) {
            // The init function also set the frequency to 100000
            i2c_init(&_i2c, sda, scl);
            /* Set _scl and _sda after i2c_init is done so that an interrupting call to init will fail because _scl and
             * _sda are NC.
             */
            _scl = scl;
            _sda = sda;

        } else {
            CORE_UTIL_ASSERT_MSG(_scl == scl && _sda == sda, "Each I2C peripheral may only be used on one set of pins");
            // Only support an I2C master on a single pair of pins
            if (_scl != scl || _sda != sda) {
                return I2CError::PinMismatch;
            }
        }
        return I2CError::None;
    }

    virtual void release()
    {
        if (mbed::util::atomic_decr<std::uint32_t>(const_cast<std::uint32_t *>(&_references), 1) == 0) {
            // i2c_free(&_i2c); // mbed-hal#71
            _sda = NC;
            _scl = NC;
        }
    }

    virtual I2CError start_segment ()
    {
        I2CTransaction * t = _TransactionQueue;
        CORE_UTIL_ASSERT(t != nullptr);
        if (!t) {
            return I2CError::NullTransaction;
        }
        I2CSegment * s = t->get_current();
        CORE_UTIL_ASSERT(s != nullptr);
        if (!s) {
            return I2CError::NullSegment;
        }
        bool stop = (s->get_next() == nullptr);
        if (s->get_dir() == I2CDirection::Transmit) {
            i2c_transfer_asynch(&_i2c, s->get_buf(), s->get_len(), nullptr, 0, t->address(),
                                stop, (uint32_t)_handler, I2C_EVENT_ALL, DMA_USAGE_NEVER);
        } else {
            i2c_transfer_asynch(&_i2c, nullptr, 0, s->get_buf(), s->get_len(), t->address(),
                                stop, (uint32_t)_handler, I2C_EVENT_ALL, DMA_USAGE_NEVER);
        }
        return I2CError::None;
    }

    virtual I2CError start_transaction()
    {
        if (i2c_active(&_i2c)) {
            return I2CError::Busy; // transaction ongoing
        }
        mbed::util::CriticalSectionLock lock;
        I2CTransaction * t = _TransactionQueue;
        CORE_UTIL_ASSERT(t != nullptr);
        if (!t) {
            return I2CError::NullTransaction;
        }
        i2c_frequency(&_i2c, t->frequency());
        t->reset_current();
        // Special case for pings:
        if (t->get_current() == nullptr) {
            i2c_transfer_asynch(&_i2c, nullptr, 0, nullptr, 0, t->address(),
                                true, (uint32_t)_handler, I2C_EVENT_ALL, DMA_USAGE_NEVER);
            return I2CError::None;
        }
        return start_segment();
    }

    virtual I2CError validate_transaction(I2CTransaction *t) const
    {
        uint16_t address = t->address();
        if (address >= 1<<10) {
            return I2CError::InvalidAddress;
        }
        return I2CError::None;
    }

    void irq_handler()
    {
        uint32_t event = i2c_irq_handler_asynch(&_i2c);
        // No further action is required if the event is 0.
        if (event) {
            process_event(event);
        }
    }

protected:
    PinName _scl;
    PinName _sda;
    i2c_t _i2c;
    const size_t _id;
    volatile uint32_t _references;
    void (*const _handler)(void);
};

template <size_t N>
struct HWI2CResourceManagers : public HWI2CResourceManagers<N-1> {
public:
    HWI2CResourceManagers() : rm(N, irq_handler_asynch) {}

private:
    HWI2CResourceManager rm;

    static void irq_handler_asynch(void)
    {
        HWI2CResourceManager *rm = static_cast<HWI2CResourceManager *>(get_i2c_owner(N));
        rm->irq_handler();
    }

public:
    I2CResourceManager * get_rm(size_t I)
    {
        CORE_UTIL_ASSERT(I <= N);
        if (I > N) {
            return nullptr;
        } else if (I == N) {
            return &rm;
        } else {
            return HWI2CResourceManagers<N-1>::get_rm(I);
        }
    }
};

template <>
struct HWI2CResourceManagers<0> {
public:
    HWI2CResourceManagers() : rm(0, irq_handler_asynch) {}

private:
    HWI2CResourceManager rm;

    static void irq_handler_asynch(void)
    {
        HWI2CResourceManager *rm = static_cast<HWI2CResourceManager *>(get_i2c_owner(0));
        rm->irq_handler();
    }

public:
    I2CResourceManager * get_rm(size_t I)
    {
        CORE_UTIL_ASSERT(I == 0);
        if (I) {
            return nullptr;
        } else {
            return &rm;
        }
    }
};

I2CResourceManager * get_i2c_owner(int I)
{
    // Trap a failed pinmap_merge()
    CORE_UTIL_ASSERT_MSG(I >=0, "The scl, sda combination must exist in the peripheral pin map");
    if (I < 0) {
        return nullptr;
    }
    // Instantiate the HWI2CResourceManager
    static struct HWI2CResourceManagers<MODULES_SIZE_I2C-1> HWManagers;
    if (I < MODULES_SIZE_I2C) {
        return HWManagers.get_rm(I);
    } else {
        CORE_UTIL_ASSERT(false);
        return nullptr;
    }
}

I2CEventHandler::I2CEventHandler():_cb(), _eventmask(0) {}

void I2CEventHandler::call(I2CTransaction *t, uint32_t event)
{
    _cb(t,event);
}

void I2CEventHandler::set(const I2C_event_callback_t &cb, uint32_t event)
{
    _cb = cb;
    _eventmask = event;
}

I2CEventHandler::operator bool() const
{
    return _eventmask && _cb;
}

} // namespace detail
} // namespace v2
} // drivers
} // namespace mbed
#endif // DEVICE_I2C && DEVICE_I2C_ASYNCH
