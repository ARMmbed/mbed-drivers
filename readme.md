## Building with yotta

mbed OS is built using yotta. So first, install [yotta](http://github.com/ARMmbed/yotta), then:

```bash
# get the mbed OS driver source code:
git clone git@github.com:mbedmicro/mbed-drivers.git
cd mbed-drivers

# build for the frdm-k64f-gcc target:
yotta target frdm-k64f-gcc

# build
yotta build
...

# fire up the debugger with the blinky test loaded
yotta debug test/mbed-test-blinky
...
> mon reset
> load
> continue

```

