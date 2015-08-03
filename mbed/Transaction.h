/* mbed Microcontroller Library
 * Copyright (c) 2015 ARM Limited
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
#ifndef MBED_TRANSACTION_H
#define MBED_TRANSACTION_H

#include "platform.h"
#include "FunctionPointer.h"
#include "mbed/Buffer.h"

namespace mbed {

/** Signature of user callback:
 *
 *    void transaction_callback(RxTxBuffer buf, int event, void *context);
 *
 * Arguments:
 *    buf: the Rx/Tx buffers that were transferred as part of the transaction
 *    event: the event that triggered the callback
 *    context: optional user provided context, propagated to the callback
 */
typedef FunctionPointer3<void, RxTxBuffer, int, void*> event_callback_t;

/** Transaction structure
 */
typedef struct {
    RxTxBuffer buffers;        /**< Buffers used in the transaction */
    uint32_t event;            /**< Event for a transaction */
    event_callback_t callback; /**< User's callback */
    void* context;             /**< User provided context */
} transaction_t;

/** Transaction callback data.
 */
typedef struct {
    RxTxBuffer buffers;        /**< buffer data */
    event_callback_t callback; /**< Callback associated with transaction */
    void *context;             /**< User context */
} transaction_cbdata_t;

/** Transaction class defines a transaction.
 */
template<typename Class>
class Transaction {
public:
    Transaction(Class *tpointer, const transaction_t& transaction) : _obj(tpointer), _data(transaction) {
    }

    Transaction() : _obj(), _data() {
    }

    ~Transaction() {
    }

    /** Get object's instance for the transaction
     *
     * @return The object which was stored
     */
    Class* get_object() {
        return _obj;
    }

    /** Get the transaction
     *
     * @return The transaction which was stored
     */
    transaction_t* get_transaction() {
        return &_data;
    }

private:
    Class* _obj;
    transaction_t _data;
};

}

#endif
