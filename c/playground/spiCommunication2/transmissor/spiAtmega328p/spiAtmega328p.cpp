#include "spiAtmega328p.hpp"

namespace {
    volatile bool busy = false;
    volatile uint8_t lastReceived = 0;
}

ISR(SPI_STC_vect) {
    busy = false;
    lastReceived = SPDR;
    Spi::spiCallbackInterrupt(SPDR);
}

void Spi::init(Mode mode, ClockRate rate) {
    // Configure pins
    if (mode == Mode::MASTER) {
        DDRB |= (1 << PB2) | (1 << PB3) | (1 << PB5); // SS, MOSI, SCK
        DDRB &= ~(1 << PB4);                         // MISO
        PORTB |= (1 << PB2);                         // SS HIGH (Master safe)
    } else {
        DDRB &= ~((1 << PB2) | (1 << PB3) | (1 << PB5)); // SS, MOSI, SCK
        DDRB |= (1 << PB4);                             // MISO
    }

    // Configure SPI control register
    SPCR = (1 << SPE);

    if (mode == Mode::MASTER) SPCR |= (1 << MSTR);

    switch (rate) {
        case ClockRate::FOSC_4:
            // nothing
            break;
        case ClockRate::FOSC_16:
            SPCR |= (1 << SPR0);
            break;
        case ClockRate::FOSC_64:
            SPCR |= (1 << SPR1);
            break;
        case ClockRate::FOSC_128:
            SPCR |= (1 << SPR0) | (1 << SPR1);
            break;
    }

    // CPOL = 0, CPHA = 1 (modo usado no seu projeto)
    SPCR &= ~(1 << CPOL);
    SPCR |= (1 << CPHA);

    SPSR &= ~(1 << SPI2X); // normal speed
}

void Spi::activateSpiCallbackInterrupt() {
    SPCR |= (1 << SPIE);
}

void Spi::disableSpiCallbackInterrupt() {
    SPCR &= ~(1 << SPIE);
}

void Spi::sendByte(uint8_t data) {
    if (!busy) {
        busy = true;
        SPDR = data;
    }
}

uint8_t Spi::getLastReceived() {
    return lastReceived;
}

bool Spi::isBusy() {
    return busy;
}

// Definição fraca para permitir override no usuário
void Spi::spiCallbackInterrupt(uint8_t) __attribute__((weak));
void Spi::spiCallbackInterrupt(uint8_t) {
    // implementado pelo usuário se quiser
}
