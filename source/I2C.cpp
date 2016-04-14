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
#include "mbed-drivers/I2C.h"
#include "minar/minar.h"

#if DEVICE_I2C

namespace mbed {

I2C *I2C::_owner = NULL;

I2C::I2C(PinName sda, PinName scl) :
#if DEVICE_I2C_ASYNCH
                                     _irq(this), _usage(DMA_USAGE_NEVER),
#endif
                                      _i2c(), _hz(100000) {
    // The init function also set the frequency to 100000
    i2c_init(&_i2c, sda, scl);

    // Used to avoid unnecessary frequency updates
    _owner = this;
}

void I2C::frequency(int hz) {
    _hz = hz;

    // We want to update the frequency even if we are already the bus owners
    i2c_frequency(&_i2c, _hz);

    // Updating the frequency of the bus we become the owners of it
    _owner = this;
}

void I2C::aquire() {
    if (_owner != this) {
        i2c_frequency(&_i2c, _hz);
        _owner = this;
    }
}

// write - Master Transmitter Mode
int I2C::write(int address, const char* data, int length, bool repeated) {
    aquire();

    int stop = (repeated) ? 0 : 1;
    int written = i2c_write(&_i2c, address, data, length, stop);

    return length != written;
}

int I2C::write(int data) {
    return i2c_byte_write(&_i2c, data);
}

// read - Master Reciever Mode
int I2C::read(int address, char* data, int length, bool repeated) {
    aquire();

    int stop = (repeated) ? 0 : 1;
    int read = i2c_read(&_i2c, address, data, length, stop);

    return length != read;
}

int I2C::read(int ack) {
    if (ack) {
        return i2c_byte_read(&_i2c, 0);
    } else {
        return i2c_byte_read(&_i2c, 1);
    }
}

void I2C::start(void) {
    i2c_start(&_i2c);
}

void I2C::stop(void) {
    i2c_stop(&_i2c);
}

#if DEVICE_I2C_ASYNCH

int I2C::transfer(int address, char *tx_buffer, int tx_length, char *rx_buffer, int rx_length, const event_callback_t& callback, int event, bool repeated) {
    return transfer(address, Buffer(tx_buffer, tx_length), Buffer(rx_buffer, rx_length), callback, event, repeated);
}

int I2C::transfer(int address, const Buffer& tx_buffer, const Buffer& rx_buffer, const event_callback_t& callback, int event, bool repeated) {
    if (i2c_active(&_i2c)) {
        return -1; // transaction ongoing
    }
    aquire();

    _current_transaction.tx_buffer = tx_buffer;
    _current_transaction.rx_buffer = rx_buffer;
    _current_transaction.callback = callback;
    int stop = (repeated) ? 0 : 1;
    _irq.callback(&I2C::irq_handler_asynch);
    i2c_transfer_asynch(&_i2c, tx_buffer.buf, tx_buffer.length, rx_buffer.buf, rx_buffer.length, address, stop, _irq.entry(), event, _usage);
    return 0;
}

void I2C::abort_transfer(void)
{
    i2c_abort_asynch(&_i2c);
}

void I2C::irq_handler_asynch(void)
{
    int event = i2c_irq_handler_asynch(&_i2c);
    if (_current_transaction.callback && event) {
        minar::Scheduler::postCallback(_current_transaction.callback.bind(_current_transaction.tx_buffer, _current_transaction.rx_buffer, event));
    }

}


#endif

} // namespace mbed

#endif
