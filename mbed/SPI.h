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
#ifndef MBED_SPI_H
#define MBED_SPI_H

#include "platform.h"

#if DEVICE_SPI

#include "spi_api.h"
#include "CThunk.h"
#include "dma_api.h"

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
    SPI(PinName mosi, PinName miso, PinName sclk, PinName _unused=NC);

    /** Configure the data transmission format
     *
     *  @param bits Number of bits per SPI frame (4 - 16)
     *  @param mode Clock polarity and phase mode (0 - 3)
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
    void format(int bits, int mode = 0);

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

    /** Start non-blocking SPI transfer using 8bit buffers.
     *
     * @param tx_buffer The TX buffer with data to be transfered. If NULL is passed,
     *                  the default SPI value is sent
     * @param tx_length The length of TX buffer
     * @param rx_buffer The RX buffer which is used for received data. If NULL is passed,
     *                  received data are ignored
     * @param rx_length The length of RX buffer
     * @param event     The logical OR of events to modify
     * @param callback  The event callback function
     * @return Zero if the transfer has started, or -1 if SPI peripheral is busy
     */
    virtual int write(uint8_t *tx_buffer, int tx_length, uint8_t *rx_buffer, int rx_length, void (*callback)(int), int event = SPI_EVENT_COMPLETE);

#if DEVICE_SPI_ASYNCH

    /** Start non-blocking SPI transfer using 16bit buffers.
     *
     * @param tx_buffer The TX buffer with data to be transfered. If NULL is passed,
     *                  the default SPI value is sent
     * @param tx_length The length of TX buffer
     * @param rx_buffer The RX buffer which is used for received data. If NULL is passed,
     *                  received data are ignored
     * @param rx_length The length of RX buffer
     * @param event     The logical OR of events to modify
     * @param callback  The event callback function
     * @return Zero if the transfer has started, or -1 if SPI peripheral is busy
     */
    virtual int write(uint16_t *tx_buffer, int tx_length, uint16_t *rx_buffer, int rx_length, void (*callback)(int), int event = SPI_EVENT_COMPLETE);

    /** Start non-blocking SPI transfer using 32bit buffers.
     *
     * @param tx_buffer The TX buffer with data to be transfered. If NULL is passed,
     *                  the default SPI value is sent
     * @param tx_length The length of TX buffer
     * @param rx_buffer The RX buffer which is used for received data. If NULL is passed,
     *                  received data are ignored
     * @param rx_length The length of RX buffer
     * @param event     The logical OR of events to modify
     * @param callback  The event callback function
     * @return Zero if the transfer has started, or -1 if SPI peripheral is busy
     */
    virtual int write(uint32_t *tx_buffer, int tx_length, uint32_t *rx_buffer, int rx_length, void (*callback)(int), int event = SPI_EVENT_COMPLETE);


    /** Configure DMA usage suggestion for non-blocking transfers
     *
     *  @param usage The usage DMA hint for peripheral
     *  @return Zero if the usage was set, -1 if a transaction is on-going
    */
    int set_dma_usage(DMAUsage usage);

protected:
    /** IRQ handler for asynchronous SPI transfers
     */
    void irq_handler_asynch(void);
    int start_write(void (*callback)(int), int event);
#endif

public:
    virtual ~SPI() {
    }

protected:
    spi_t _spi;
    CThunk<SPI> _irq;
    void (*_user_callback)(int event);
    DMAUsage _usage;

    void aquire(void);
    static SPI *_owner;
    int _bits;
    int _mode;
    int _hz;
};

} // namespace mbed

#endif

#endif
