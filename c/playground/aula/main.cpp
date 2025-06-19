
#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibGpioBus.hpp"
#include "funsape/peripheral/funsapeLibGpioPin.hpp"
#include "funsape/peripheral/funsapeLibTimer1.hpp"
#include "funsape/device/funsapeLibHd44780.hpp"

volatile Edge currentEdge = Edge::FALLING;
vuint16_t highTime = 0;
vuint16_t period = 0;
vbool_t newDataReady = false;

int main()
{
    // Local variables
    GpioBus gpioLcdData;
    GpioPin gpioLcdEn;
    GpioPin gpioLcdRs;
    GpioPin gpioLcdRw;
    Hd44780 lcd;

    // LED configuration
    setBit(DDRB, PB5);

    // GPIO LCD configuration
    gpioLcdData.init(&DDRD, GpioBus::PinIndex::P4, 4);
    gpioLcdEn.init(&DDRD, GpioPin::PinIndex::P3);
    gpioLcdRs.init(&DDRD, GpioPin::PinIndex::P2);
    gpioLcdRw.init(&DDRD, GpioPin::PinIndex::P1);

    // LCD configuration
    lcd.setDataPort(&gpioLcdData);
    lcd.setControlPort(&gpioLcdEn, &gpioLcdRs, &gpioLcdRw);
    lcd.init(Hd44780::Size::LCD_16X2);
    lcd.stdio();
    printf("TESTE\n\n");

    // TIMER1 configuration
    timer1.setInputCaptureValue(0);
    timer1.setInputCaptureMode(currentEdge, false);
    timer1.init(Timer1::Mode::NORMAL, Timer1::ClockSource::PRESCALER_64);
    timer1.clearInputCaptureInterruptRequest();
    timer1.clearOverflowInterruptRequest();
    timer1.activateInputCaptureInterrupt();

    sei();

    while(true) {
        if(newDataReady) {
            lcd.cursorHome();
            printf("%u\n%u\n", highTime, period);
            newDataReady = false;
        }
        delayMs(100);
    }

    return 0;
}

void timer1InputCaptureCallback(void)
{
    if(currentEdge == Edge::RISING) {
        timer1.setCounterValue(0);
        period = timer1.getInputCaptureValue();
        timer1.setInputCaptureMode(Edge::FALLING, false);
        currentEdge = Edge::FALLING;
        newDataReady = true;
    } else {
        highTime = timer1.getInputCaptureValue();
        timer1.setInputCaptureMode(Edge::RISING, false);
        currentEdge = Edge::RISING;
    }
}

