#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibTimer1.hpp"
#include "funsape/peripheral/funsapeLibTimer0.hpp"

int main()
{
    // Timer1 configurado em modo CTC com prescaler 1024
    timer1.init(Timer1::Mode::CTC_OCRA, Timer1::ClockSource::PRESCALER_1024);

    // Timer0 configurado em modo CTC com prescaler 256
    timer0.init(Timer0::Mode::CTC_OCRA, Timer0::ClockSource::PRESCALER_256);

    // Valor de comparacao para 1Hz
    timer1.setCompareAValue(15624);

    // Valor de comparacao para 250Hz
    timer0.setCompareAValue(250);

    // Saida em modo toggle no pino PB1
    timer1.setOutputMode(Timer1::OutputMode::TOGGLE_ON_COMPARE, Timer1::OutputMode::NORMAL);

    // Saida em modo toggle no pino PD6
    timer0.setOutputMode(Timer0::OutputMode::TOGGLE_ON_COMPARE, Timer0::OutputMode::NORMAL);

    setBit(DDRB, PB1);
    setBit(DDRD, PD6);

    while(1) {

    }

    return 0;
}
