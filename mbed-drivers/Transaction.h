/*
 * Copyright (c) 2015-2016, ARM Limited, All Rights Reserved
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
#ifndef MBED_TRANSACTION_H
#define MBED_TRANSACTION_H

#include "platform.h"
#include "core-util/FunctionPointer.h"
#include "mbed-drivers/Buffer.h"

namespace mbed {

/** Transactions in one direction (RX or TX)
 */
template <typename Callback>
struct OneWayTransaction {
    Buffer buffer;             /**< Transaction buffer */
    uint32_t event;            /**< Event for a transaction */
    Callback callback;         /**< User's callback */
};

/** Transactions in two directions (RX and TX)
 */
template <typename Callback>
struct TwoWayTransaction {
    Buffer tx_buffer;          /**< Transmit buffer */
    Buffer rx_buffer;          /**< Receive buffer */
    uint32_t event;            /**< Event for a transaction */
    Callback callback;         /**< User's callback */
};

/** Transaction class defines a transaction.
 */
template<typename Class, typename TransactionData>
class Transaction {
public:
    Transaction(Class *tpointer, const TransactionData& transaction) : _obj(tpointer), _data(transaction) {
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
    TransactionData* get_transaction() {
        return &_data;
    }

private:
    Class* _obj;
    TransactionData _data;
};

} // namespace mbed

#endif // #ifndef MBED_TRANSACTION_H

