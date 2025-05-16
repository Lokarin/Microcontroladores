#include <avr/io.h>
#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibGpioPin.hpp"
#include "funsape/peripheral/funsapeLibTimer0.hpp"

int main()
{
    //Objeto led
    GpioPin led;

    //Configurando objeto led
    led.init(&DDRB, GpioPin::PinIndex::P5);
    led.setMode(GpioPin::Mode::OUTPUT_PUSH_PULL);

    //Timer0 config
    timer0.init(Timer0::Mode::PWM_FAST_OCRA, Timer0::ClockSource::PRESCALER_1);

    timer0.setCompareAValue(15);
    timer0.setCompareBValue(15);

    timer0.setOutputMode(Timer0::OutputMode::TOGGLE_ON_COMPARE, Timer0::OutputMode::TOGGLE_ON_COMPARE);

    setBit(DDRD, PD6);
    setBit(DDRD, PD5);

    while(1) {
        led.toggle();
        delayMs(250);
    }

    return 0;
}
