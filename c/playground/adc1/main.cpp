#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibAdc.hpp"

int main()
{
    setBit(DDRB, PB5);

    while(1) {
        delayMs(1000);
        cplBit(PORTB, PB5);

    }

    return 0;
}
