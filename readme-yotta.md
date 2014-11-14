## Building with yotta

First install [yotta](http://github.com/ARMmbed/yotta) >= 0.0.28, then:

```bash
# install and link the yotta target (necessary because the target isn't published)
git clone git@github.com:ARM-RD/target-stk3700-gcc.git
cd target-stk3700-gcc
yotta link-target

# get the mbed_private source, switch onto the right branch
cd .. 
git clone git@github.com:mbedmicro/mbed_private.git
cd mbed_private
git checkout dev_async_hal_yotta

# use the target we installed earlier
yotta target stk3700-gcc
yotta link-target stk3700-gcc

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

