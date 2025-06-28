#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/device/funsapeLibHd44780.hpp"
#include "spiAtmega328p/spiAtmega328p.hpp"

Hd44780 display;
vuint8_t spiReceived = 0;
volatile bool_t dataReady = false;

//void spi_init_slave() {
//    // Pinos como entrada
//    clrBit(DDRB, PB2); // SS
//    clrBit(DDRB, PB3); // MOSI
//    clrBit(DDRB, PB5); // SCK
//    setBit(DDRB, PB4); // MISO
//    
//    // SPI Enable, Slave
//    setBit(SPCR, SPIE);     // Enable de interrupções, ativo em 1
//    setBit(SPCR, SPE);      // Enable do SPI, ativo em 1
//    clrBit(SPCR, DORD);     // Ordem dos bits. 1 para começar por MSB
//    clrBit(SPCR, MSTR);     // Master/Slave select. 1 para Master
//    clrBit(SPCR, CPOL);     // Polariação do SCK. 1 para HIGH em idle
//    setBit(SPCR, CPHA);     // Fase do clock. Em 1, Coloca o bit em MOSI na subida
//
//    clrBit(SPSR, SPI2X);    // Double Speed. Ativo em 1.
//}

int main()
{
    // Variables
    GpioPin gpioDisplayEn;
    GpioPin gpioDisplayRs;
    GpioPin gpioDisplayRw;
    GpioBus gpioDisplayData;

    // Configure SPI
    Spi::init(Spi::Mode::SLAVE, Spi::ClockRate::FOSC_4);

    // Configure Gpio Pins
    gpioDisplayEn.init(&DDRD, GpioPin::PinIndex::P0);
    gpioDisplayRs.init(&DDRD, GpioPin::PinIndex::P1);
    gpioDisplayRw.init(&DDRD, GpioPin::PinIndex::P2);
    gpioDisplayData.init(&DDRD, GpioBus::PinIndex::P3, 4);

    // Configure Display
    display.setControlPort(
            &gpioDisplayEn, 
            &gpioDisplayRs,
            &gpioDisplayRw
    );
    display.setDataPort(&gpioDisplayData);
    display.init(Hd44780::Size::LCD_16X2);
    display.stdio();
    //printf("--Teste do LCD--\n0123456789ABCDEF\n");
    //delayMs(2000);
    //display.clearScreen();

    sei();

    while(true) {
        if (dataReady) {
            uint16_t dataFull = spiReceived << 2;
            display.clearScreen();
            printf("ADC: %u", dataFull);
            dataReady = false;
        }
        delayMs(300);
    }

    return 0;
}

//ISR(SPI_STC_vect) {
//    spiReceived = SPDR;
//    dataReady = true;
//}

void Spi::spiCallbackInterrupt(uint8_t received) {
    spiReceived = received;
    dataReady = true;
}
