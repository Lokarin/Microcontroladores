#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibPcint0.hpp"


int main()
{
    setBit(DDRB, PB5);

    pcint0.init(Pcint0::Pin::PIN_PCINT0);
    pcint0.clearInterruptRequest();
    pcint0.activateInterrupt();

    sei();

    return 0;
}

void pcint0InterruptCallback(void) {
    cplBit(PORTB, PB5);
}
