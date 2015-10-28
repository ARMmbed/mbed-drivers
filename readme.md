# mbed-drivers

This module is one of the main 'entry points' into mbed OS. It contains the definition and implementation of the
high level (user facing) API for the MCU peripherals, as well as the implementation of the `main` function
(check [the MINAR documentation](https://github.com/ARMmbed/minar) for more details about the usage of `main`
in mbed OS).

Because of this, you'll always want to make `mbed-drivers` a dependency of your mbed OS application. Check
[our user guide](https://docs.mbed.com/docs/getting-started-mbed-os/) for more details about `mbed-drivers` and
mbed OS in general.

