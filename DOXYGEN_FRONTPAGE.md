
# mbed-drivers

This module is one of the main 'entry points' into mbed OS. It contains the definition and implementation of the
high level (user facing) API for the MCU peripherals, as well as the implementation of the `main` function
(mbed OS applications use `app_start` instead of `main`; check ["Writing applications for mbed OS"](https://docs.mbed.com/docs/getting-started-mbed-os/en/latest/Full_Guide/app_on_yotta/#writing-applications-for-mbed-os)
for more details).

Because of this, you'll always want to make `mbed-drivers` a dependency of your mbed OS application. Check
[our "Getting started" guide](https://docs.mbed.com/docs/getting-started-mbed-os/) for more details about `mbed-drivers`
and mbed OS in general.

# Version 1.0.0
This release includes version 1.0.0 of the mbed-drivers API. The current mbed-drivers API is an evolution of the original mbed SDK and, as such, there are places where its design is not well suited to the event-driven programming model used in the rest of mbed OS. We anticipate significant changes in the mbed-drivers API over coming releases, but we are publishing the current API as-is, so that developers can start working with mbed OS today.

From this point on, we will follow the familiar versioning rules described by semver (semver.org) when publishing updates to mbed-drivers. If we make a breaking change to the API, we will increase the major revision number. We will also aim to provide backwards compatibility by developing different version of the API in different C++ namespaces.

We have ambitious plans for mbed OS and that means making changes in mbed-drivers. By following this path we hope we can help developers get started with mbed OS today, and yet still allow us to make meaningful API design improvements for the future.

### STDIO retargeting

The mbed-drivers defines retargeting of stdin, stdout and stderr to UART. The default baudrate for STDIO UART peripheral is set via ```YOTTA_CFG_MBED_OS_STDIO_DEFAULT_BAUD```. If this yotta config is not defined, the default value is 115200.

To change STDIO serial settings in the runtime, retrieve the Serial STDIO object ```get_stdio_serial()```.

```
Serial& pc = get_stdio_serial();
pc.baud(9600);
```
