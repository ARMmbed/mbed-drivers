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
#include "mbed-drivers/I2CSlave.h"
#include "minar/minar.h"

#if DEVICE_I2CSLAVE

namespace mbed {
  
I2CSlave *I2CSlave::_owner = NULL;

I2CSlave::I2CSlave(PinName sda, PinName scl) : 
  #if DEVICE_I2C_ASYNCH
                                     _irq(this), _usage(DMA_USAGE_NEVER),
#endif
                                    _i2c() {
    i2c_init(&_i2c, sda, scl);
    i2c_frequency(&_i2c, 100000);
    i2c_slave_mode(&_i2c, 1);
                                      
    // Used to avoid unnecessary frequency updates
    _owner = this;
}

void I2CSlave::frequency(int hz) {
    i2c_frequency(&_i2c, hz);
}

void I2CSlave::aquire() {
    if (_owner != this) {
        i2c_frequency(&_i2c, _hz);
        _owner = this;
    }
}

void I2CSlave::address(int address) {
    int addr = (address & 0xFF) | 1;
    i2c_slave_address(&_i2c, 0, addr, 0);
}

int I2CSlave::receive(void) {
    return i2c_slave_receive(&_i2c);
}

int I2CSlave::read(char *data, int length) {
    return i2c_slave_read(&_i2c, data, length) != length;
}

int I2CSlave::read(void) {
    return i2c_byte_read(&_i2c, 0);
}

int I2CSlave::write(const char *data, int length) {
    return i2c_slave_write(&_i2c, data, length) != length;
}

int I2CSlave::write(int data) {
    return i2c_byte_write(&_i2c, data);
}

void I2CSlave::stop(void) {
    i2c_stop(&_i2c);
}

#if DEVICE_I2C_ASYNCH

int I2CSlave::transfer(int address, char *tx_buffer, int tx_length, char *rx_buffer, int rx_length, const event_callback_t& callback, int event, bool repeated) {
    return transfer(address, Buffer(tx_buffer, tx_length), Buffer(rx_buffer, rx_length), callback, event, repeated);
}

int I2CSlave::transfer(int address, const Buffer& tx_buffer, const Buffer& rx_buffer, const event_callback_t& callback, int event, bool repeated) {
    if (i2c_active(&_i2c)) {
        return -1; // transaction ongoing
    }
    aquire();

    _current_transaction.tx_buffer = tx_buffer;
    _current_transaction.rx_buffer = rx_buffer;
    _current_transaction.callback = callback;
    int stop = (repeated) ? 0 : 1;
    _irq.callback(&I2CSlave::irq_handler_asynch);
    i2cslave_transfer_asynch(&_i2c, tx_buffer.buf, tx_buffer.length, rx_buffer.buf, rx_buffer.length, address, stop, _irq.entry(), event, _usage);
    return 0;
}

void I2CSlave::abort_transfer(void)
{
    i2c_abort_asynch(&_i2c);
}

void I2CSlave::irq_handler_asynch(void)
{
    int event = i2cslave_irq_handler_asynch(&_i2c);
    if (_current_transaction.callback && event) {
        minar::Scheduler::postCallback(_current_transaction.callback.bind(_current_transaction.tx_buffer, _current_transaction.rx_buffer, event));
    }

}

void I2CSlave::toto_handler_asynch(void)
{
    int event = i2cslave_irq_handler_asynch(&_i2c);
    if (_current_transaction.callback && event) {
        minar::Scheduler::postCallback(_current_transaction.callback.bind(_current_transaction.tx_buffer, _current_transaction.rx_buffer, event));
    }

}

#endif

}

#endif
