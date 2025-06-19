
#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibGpioBus.hpp"
#include "funsape/peripheral/funsapeLibGpioPin.hpp"
#include "funsape/peripheral/funsapeLibTimer1.hpp"
#include "funsape/device/funsapeLibHd44780.hpp"

int main()
{
    // Local variables
    GpioBus gpioLcdData;
    GpioPin gpioLcdEn;
    GpioPin gpioLcdRs;
    Hd44780 lcd;

    // LED configuration
    setBit(DDRB, PB5);

    // GPIO LCD configuration
    gpioLcdData.init(&DDRC, GpioBus::PinIndex::P0, 3);
    gpioLcdRs.init(&DDRC, GpioPin::PinIndex::P4);
    gpioLcdEn.init(&DDRC, GpioPin::PinIndex::P5);

    // LCD configuration
    lcd.setDataPort(&gpioLcdData);
    lcd.setControlPort(&gpioLcdEn, &gpioLcdRs);
    lcd.init(Hd44780::Size::LCD_16X2);
    lcd.stdio();
    printf("TESTE\n\n");

    sei();

    while(true) {
        cplBit(PORTB, PB5);
        delayMs(100);
    }

    return 0;
}

