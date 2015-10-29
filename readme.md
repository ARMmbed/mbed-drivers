
# mbed-drivers

This module is one of the main 'entry points' into mbed OS. It contains the definition and implementation of the
high level (user facing) API for the MCU peripherals, as well as the implementation of the `main` function
(mbed OS applications use `app_start` instead of `main`; check ["Writing applications for mbed OS"](https://docs.mbed.com/docs/getting-started-mbed-os/en/latest/Full_Guide/app_on_yotta/#writing-applications-for-mbed-os)
for more details).

Because of this, you'll always want to make `mbed-drivers` a dependency of your mbed OS application. Check
[our "Getting started" guide](https://docs.mbed.com/docs/getting-started-mbed-os/) for more details about `mbed-drivers`
and mbed OS in general.
