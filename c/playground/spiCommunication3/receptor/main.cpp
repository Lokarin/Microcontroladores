#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "spi/atmega328pSpi.hpp"

volatile bool_t dataReady = false;

int main() {
    // Configure SPI
    Spi::init(Spi::Mode::MASTER, Spi::ClockRate::FOSC_4, Spi::DataMode::MODE_1);
    Spi::activateSpiCallbackInterrupt();

    // Debug
    setBit(DDRC, PC5);
    clrBit(PORTC, PC5);

    sei();

    while(true) {
    }

    return 0;
}

void Spi::spiCallbackInterrupt(uint8_t received) {
    dataReady = true;
}
