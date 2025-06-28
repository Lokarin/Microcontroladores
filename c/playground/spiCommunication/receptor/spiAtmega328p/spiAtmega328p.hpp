#ifndef SPI_HPP
#define SPI_HPP

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdbool.h>

namespace Spi {

    enum class Mode {
        MASTER,
        SLAVE
    };

    enum class ClockRate {
        FOSC_4,
        FOSC_16,
        FOSC_64,
        FOSC_128
    };

    void init(Mode mode, ClockRate rate);
    void sendByte(uint8_t data);
    bool isBusy();
    void spiCallbackInterrupt(uint8_t received); // Função que o usuário pode implementar

} // namespace Spi

#endif // SPI_HPP
