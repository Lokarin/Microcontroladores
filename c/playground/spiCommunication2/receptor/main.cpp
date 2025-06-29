#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibInt0.hpp"
#include "funsape/peripheral/funsapeLibTimer1.hpp"
#include "funsape/device/funsapeLibHd44780.hpp"
#include "spiAtmega328p/spiAtmega328p.hpp"

Hd44780 display;
volatile bool_t dataReady = false;

int main()
{
    // Variables
    GpioPin gpioDisplayEn;
    GpioPin gpioDisplayRs;
    GpioPin gpioDisplayRw;
    GpioBus gpioDisplayData;

    // Configure Timer0
    timer1.setCompareAValue(4687-1);
    timer1.init(Timer1::Mode::CTC_OCRA, Timer1::ClockSource::PRESCALER_1024);
    timer1.activateCompareAInterrupt();

    // Configure SPI
    Spi::init(Spi::Mode::MASTER, Spi::ClockRate::FOSC_4);
    Spi::activateSpiCallbackInterrupt();

    // Configure INT0
    int0.init(Int0::SenseMode::FALLING_EDGE);
    int0.activateInterrupt();

    // Configure Gpio Pins
    gpioDisplayEn.init(&DDRD, GpioPin::PinIndex::P0);
    gpioDisplayRs.init(&DDRD, GpioPin::PinIndex::P1);
    gpioDisplayRw.init(&DDRB, GpioPin::PinIndex::P0);
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

    // SS do Slave
    setBit(DDRB, PB1);
    clrBit(PORTB, PB1);

    sei();

    while(true) {
    }

    return 0;
}

void int0InterruptCallback() {
    Spi::sendByte(0);
}

void Spi::spiCallbackInterrupt(uint8_t received) {
    dataReady = true;
}

void timer1CompareACallback() {
        if (dataReady) {
            uint16_t dataFull = Spi::getLastReceived() << 2;
            display.clearScreen();
            printf("ADC: %u", dataFull);
            dataReady = false;
        }
}
