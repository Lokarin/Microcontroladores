#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibGpioBus.hpp"
#include "funsape/peripheral/funsapeLibGpioPin.hpp"
#include "funsape/device/funsapeLibHd44780.hpp"
#include "funsape/peripheral/funsapeLibTimer1.hpp"
#include "funsape/peripheral/funsapeLibTimer0.hpp"


Hd44780 display;

vuint16_t tSubida = 0;
vuint16_t tDescida = 0;
vuint16_t tNxtSubida = 0;

volatile bool_t aguardandoSubida = true;
volatile bool_t aguardandoDescida = false;
volatile bool_t aguardandoNxtSubida = false;
volatile bool_t capturaCompleta = false;

vuint8_t contadorTimer = 0;
vuint16_t tempCent = 0;

int main()
{
    // Variables
    GpioPin gpioDisplayEn;
    GpioPin gpioDisplayRs;
    GpioPin gpioDisplayRw;
    GpioBus gpioDisplayData;

    // Configure Gpio Pins
    gpioDisplayEn.init(&DDRD, GpioPin::PinIndex::P0);
    gpioDisplayRs.init(&DDRD, GpioPin::PinIndex::P1);
    gpioDisplayRw.init(&DDRD, GpioPin::PinIndex::P2);
    gpioDisplayData.init(&DDRD, GpioBus::PinIndex::P3, 4);

    // Configure LED
    setBit(DDRB, PB5);

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

    // Configure Timer1
    timer1.setInputCaptureMode(Edge::RISING, false);
    timer1.activateInputCaptureInterrupt();
    timer1.init(Timer1::Mode::NORMAL, Timer1::ClockSource::PRESCALER_64);

    // Configure Timer0
    timer0.setCompareAValue(156 - 1);
    timer0.init(Timer0::Mode::CTC_OCRA, Timer0::ClockSource::PRESCALER_1024);
    timer0.activateCompareAInterrupt();
    timer0.clearCompareAInterruptRequest();

    sei();

    while(true) {

    }

    return 0;
}

void timer1InputCaptureCallback(void) {
    uint16_t tempoCapturado = timer1.getInputCaptureValue();

    if (aguardandoSubida) {
        tSubida = tempoCapturado;
        timer1.setInputCaptureMode(Edge::FALLING, false);

        aguardandoSubida = false;
        aguardandoDescida = true;

    } else if (aguardandoDescida) {
        tDescida = tempoCapturado;
        timer1.setInputCaptureMode(Edge::RISING, false);

        aguardandoDescida = false;
        aguardandoNxtSubida = true;

    } else if (aguardandoNxtSubida) {
        tNxtSubida = tempoCapturado;

        aguardandoNxtSubida = false;
        aguardandoSubida = true;

        capturaCompleta = true;
    }
}

void timer0CompareACallback(void) {
    if (capturaCompleta) {
        capturaCompleta = false;

        uint16_t tempoAlto = tDescida - tSubida;
        uint16_t periodo = tNxtSubida - tSubida;

        if (periodo != 0) {
            tempCent = (uint32_t)tempoAlto * 11000 / periodo;
        }
    }

    // contar 20 vezes 10ms
    if (++contadorTimer >= 20) {
        contadorTimer = 0;
        display.clearScreen();
        printf("Temp: %u.%02u C\n",
               tempCent / 100,
               tempCent % 100);
    }
}
