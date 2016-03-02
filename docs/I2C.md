**Warning: this API is experimental and may be subject to change**

mbed OS has two I2C APIs. This article covers the new, experimental version, which can be found on [mbed-drivers/v2](https://github.com/ARMmbed/mbed-drivers/blob/master/mbed-drivers/v2/I2C.hpp). The stable version is at [mbed-drivers](https://github.com/ARMmbed/mbed-drivers/blob/master/mbed-drivers/I2C.h).

# Experimental version 2 asynchronous I2C
I2C is a 2-wire serial bus protocol, designed to provide easy communication between peripherals on the same circuit board.

The I2C class provides an asynchronous usage model for I2C master mode.

In order to facilitate sharing an I2C master mode controller between drivers for connected peripherals, an I2C resource manager is required. This resource manager provides a single interface: `post_transaction()`, an API which adds a transaction to the transaction queue. The `I2C` class interfaces with an I2C Resource manager in order to initiate transactions and receive events. The `I2CTransaction` class encapsulates all I2C transaction parameters. The `I2CResourceManager` class is a generic interface for implementing I2C resource managers.

Different resource managers are required for each kind of I2C master. By default, only the resource manager matching the on-chip I2C master is provided.

Transaction queues are composed of one or more transmit or receive segments, each of which contains a buffer, a direction and, optionally, an callback to execute in IRQ context between finishing the current segment and starting the next.

Typically, transactions should only be created in non-IRQ context, since they require allocating memory. However, if two pool allocators (one for the `I2CTransaction` and one for the `I2CSegments`) is provided to the `I2C` object on construction, it can use `transfer_to_irqsafe` to build a transaction in IRQ context.

# I2C
I2C encapsulates an I2C master. The physical I2C master to use is selected via the pins provided to the constructor. The `frequency()` API sets the default frequency for transactions issued from the I2C object. This is used for each transaction issued by I2C unless overridden when creating the transaction. Transactions are initiated by calling `transfer_to()` or ```transfer_to_irqsafe()```. Both of these APIs create an instance of the `TransferAdder` helper class.

# TransferAdder
The TransferAdder class allows convenient construction of complex transfers.

The `frequency()` member overrides the default frequency set by the issuing I2C object.

The `on()` member allows setting up to 4 event handlers, each with a corresponding event mask.

The `tx()` members add a buffer to send to the transfer.

The `rx()` members add a buffer to receive into to the transfer. There is a special case of `rx()`, which doesn't use a normal buffer. When `rx(size_t)` is called with a size of less than 8 bytes, the underlying EphermeralBuffer is placed in ephemeral mode. This means that no preallocated receive buffer is needed, instead the data is packed directly into the EphemeralBuffer. This has a side-effect that the data will be freed once the last event handler has exited, so if the data must be retained, it should be copied out.

The `apply()` method validates the transfer and adds it to the transaction queue of the I2CResourceManager. It returns the result of validation.

# I2C resource managers
I2C Resource managers are instantiated statically and initialized on first use. There is one Resource Manager per logical I2C master. Logical I2C masters could consist of:

* Onchip I2C masters
* I2C Bridges (SPI->I2C bridge, I2C->I2C bridge, etc.)
* Bit banged I2C
* Bit banged I2C over SPI GPIO expander
* More...

Currently only onchip I2C masters are supported.

The I2CResourceManager manager is a multiplexer that guarantees mutually exclusive access to the underlying hardware I2C master. It does this by serializing transactions and ensuring that they are processed atomically. This way, there can be many users of the I2C bus, without access conflicts.

The I2CResourceManager is the interface required to manage a particular I2C master. It provides several common operations, but explicitly does not permit copy or move construction or assignment.

The common operations provided by the Resource manager are:

* Adding a transaction to the queue
* Processing an event

These operations are common to all resource managers, so they are provided by the interface class.

## Event handling overview

When an interrupt occurs, the HAL processes it; if anything needs to be handled by the upper layers, an event is generated. The derived resource manager should call I2CResourceManager::process_event with this event. If appropriate, the I2CResourceManager will call the I2CSegment's irq callback handler.

The resource manager handles events according to the following pseudocode:

```
If there is an error condition:
     Schedule handle_event() with the current transaction and event
Otherwise, if there are more segments to process:
     Start the next segment
Otherwise,
     Schedule handle_event() with the current transaction and the done flag
If another segment was not started,
     Start the next transaction
```

`handle_event()` calls the registered event handlers for the current transaction, then frees the Transaction, using the I2C object that originally issued the transaction.

# I2C transactions
An I2CTransaction contains a list of event handlers and their event masks, an I2C address, an operating frequency, and zero or more I2CSegments. Zero-segment Transactions are explicitly supported since they are useful in connected device discovery (pings).

# I2C Segments
An I2CSegment is a wrapper around an EphemeralBuffer. It provides an I2C transfer direction (read or write) and an optional callback to execute in IRQ context. I2CSegments also provide a chaining pointer so that they can perform sequential or scatter/gather operations.

# Example: constructing I2C transactions

```C++
#include "mbed-drivers/mbed.h"
#include "mbed-drivers/v2/I2C.hpp"

void doneCB(bool dir, I2CTransaction *t, uint32_t event) {
    // Do something
}
mbed::drivers::v2::I2C i2c0(sda, scl);
void app_start (int, char **) {
    static uint8_t cmd[2] = {0xaa, 0x55};
    i2c0.transfer_to(addr).tx(cmd,2).rx(4).on(I2C_EVENT_ALL, doneCB);
}
```

# Example: Handling I2C events

```C++
// Read 6 bytes from I2C EEPROM slave at address 0x62

#include "mbed-drivers/mbed.h"
#include "mbed-drivers/v2/I2C.hpp"

mbed::drivers::v2::I2C i2c(p28, p27);

// This callback executes in minar context
void xfer_done(mbed::drivers::v2::I2CTransaction * t, int event) {
    t->reset_current(); // Set the current pointer to root.
    uint8_t *txPtr = t->get_current->get_buf();
    printf("EEPROM 0x%02x@0x%02x%02x: ", t->address(), txPtr[0], txPtr[1]);
    // Get the rx buffer pointer
    uint8_t * rxPtr = t->get_current() // get the first segment (the tx segment)
                       ->get_next()    // get the second segment (the rx segment)
                       ->get_buf();    // get the buffer pointer
    for (uint i = 0; i < 6; i++) {
        printf("%02x", rxPtr[i]);
    }
    printf("\n");
    // Both the tx and rx buffers are ephemeral, so they will be freed automatically when this function exits
}

void app_start(int, char **) {
    i2c.transfer_to(0x62)           // I2C Slave Address
       .tx_ephemeral("\x12\x34", 2) // Send EEPROM location
       .rx(6)                       // Read 6 bytes into an ephemeral buffer
       .on(I2C_EVENT_TRANSFER_COMPLETE, xfer_done)
       .apply();
}
```
