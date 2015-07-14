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
#ifndef MBED_BUSOUT_H
#define MBED_BUSOUT_H

#include "DigitalOut.h"

namespace mbed {

/** A digital output bus, used for setting the state of a collection of pins
 */
class BusOut {

public:

    /** Create an BusOut, connected to the specified pins
     *
     *  @param p<n> DigitalOut pin to connect to bus bit <n>
     *
     *  @note
     *  It is only required to specify as many pin variables as is required
     *  for the bus; the rest will default to NC (not connected)
     */
    BusOut(PinName p0, PinName p1 = NC, PinName p2 = NC, PinName p3 = NC,
           PinName p4 = NC, PinName p5 = NC, PinName p6 = NC, PinName p7 = NC,
           PinName p8 = NC, PinName p9 = NC, PinName p10 = NC, PinName p11 = NC,
           PinName p12 = NC, PinName p13 = NC, PinName p14 = NC, PinName p15 = NC);

    BusOut(PinName pins[16]);

    virtual ~BusOut();

    /** Write the value to the output bus
     *
     *  @param value An integer specifying a bit to write for every corresponding DigitalOut pin
     */
    void write(int value);

    /** Read the value currently output on the bus
     *
     *  @returns
     *    An integer with each bit corresponding to associated DigitalOut pin setting
     */
    int read();

    /** A shorthand for write()
     */
    BusOut& operator= (int v);
    BusOut& operator= (BusOut& rhs);

    /** A shorthand for read()
     */
    operator int();

protected:
    DigitalOut* _pin[16];

   /* disallow copy constructor and assignment operators */
private:
    BusOut(const BusOut&);
    BusOut & operator = (const BusOut&);
};

} // namespace mbed

#endif
