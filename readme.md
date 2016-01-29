
# mbed-drivers

This module is one of the main 'entry points' into mbed OS. It contains the definition and implementation of the
high level (user facing) API for the MCU peripherals, as well as the implementation of the `main` function
(mbed OS applications use `app_start` instead of `main`; check ["Writing applications for mbed OS"](https://docs.mbed.com/docs/getting-started-mbed-os/en/latest/Full_Guide/app_on_yotta/#writing-applications-for-mbed-os)
for more details).

Because of this, you'll always want to make `mbed-drivers` a dependency of your mbed OS application. Check
[our "Getting started" guide](https://docs.mbed.com/docs/getting-started-mbed-os/) for more details about `mbed-drivers`
and mbed OS in general.

### STDIO retargeting

The mbed-drivers defines retargeting of stdin, stdout and stderr to UART. The default baudrate for STDIO UART peripheral is set via ```YOTTA_CFG_MBED_OS_STDIO_DEFAULT_BAUD```. If this yotta config is not defined, the default value is 115200.

To change STDIO serial settings in the runtime, retrieve the Serial STDIO object ```get_stdio_serial()```.

```
Serial& pc = get_stdio_serial();
pc.baud(9600);
```
