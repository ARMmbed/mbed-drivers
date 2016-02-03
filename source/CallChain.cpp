/*
 * Copyright (c) 2013-2016, ARM Limited, All Rights Reserved
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
#include "mbed-drivers/CallChain.h"
#include "cmsis.h"

namespace mbed {

CallChain::CallChain(int size) : _chain(), _size(size), _elements(0) {
    _chain = new pFunctionPointer_t[size]();
}

CallChain::~CallChain() {
    clear();
    delete _chain;
}

pFunctionPointer_t CallChain::add(void (*function)(void)) {
    return common_add(new mbed::util::FunctionPointer(function));
}

pFunctionPointer_t CallChain::add_front(void (*function)(void)) {
    return common_add_front(new mbed::util::FunctionPointer(function));
}

int CallChain::size() const {
    return _elements;
}

pFunctionPointer_t CallChain::get(int i) const {
    if (i < 0 || i >= _elements)
        return NULL;
    return _chain[i];
}

int CallChain::find(pFunctionPointer_t f) const {
    for (int i = 0; i < _elements; i++)
        if (f == _chain[i])
            return i;
    return -1;
}

void CallChain::clear() {
    for(int i = 0; i < _elements; i ++) {
        delete _chain[i];
        _chain[i] = NULL;
    }
    _elements = 0;
}

bool CallChain::remove(pFunctionPointer_t f) {
    int i;

    if ((i = find(f)) == -1)
        return false;
    if (i != _elements - 1)
        memmove(_chain + i, _chain + i + 1, (_elements - i - 1) * sizeof(pFunctionPointer_t));
    delete f;
    _elements --;
    return true;
}

void CallChain::call() {
    for(int i = 0; i < _elements; i++)
        _chain[i]->call();
}

void CallChain::_check_size() {
    if (_elements < _size)
        return;
    _size = (_size < 4) ? 4 : _size + 4;
    pFunctionPointer_t* new_chain = new pFunctionPointer_t[_size]();
    memcpy(new_chain, _chain, _elements * sizeof(pFunctionPointer_t));
    delete _chain;
    _chain = new_chain;
}

pFunctionPointer_t CallChain::common_add(pFunctionPointer_t pf) {
    _check_size();
    _chain[_elements] = pf;
    _elements ++;
    return pf;
}

pFunctionPointer_t CallChain::common_add_front(pFunctionPointer_t pf) {
    _check_size();
    memmove(_chain + 1, _chain, _elements * sizeof(pFunctionPointer_t));
    _chain[0] = pf;
    _elements ++;
    return pf;
}

} // namespace mbed
