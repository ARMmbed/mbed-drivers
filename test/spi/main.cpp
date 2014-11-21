#include "mbed.h"

#if defined(TARGET_LIKE_STK3700)
SPI spi(PD0, PD1, PD2);
DigitalOut latchpin(PD3);

#else // defined(TARGET_LIKE_STK3700)
SPI spi(p11, p12, p13);
DigitalOut latchpin(p10);
#endif // else defined(TARGET_LIKE_STK3700)

int main() {
    spi.format(8, 0);
    spi.frequency(16 * 1000 * 1000);

    latchpin = 0;
    while (1) {
        latchpin = 1;
        spi.write(0);
        latchpin = 0;
    }
}
