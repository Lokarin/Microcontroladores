#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibTimer1.hpp"

int main()
{
    // Timer1 configurado em modo CTC com prescaler 1024
    timer1.init(Timer1::Mode::CTC_OCRA, Timer1::ClockSource::PRESCALER_1024);

    // Valor de comparacao para 1Hz
    timer1.setCompareAValue(15624);

    // Saida em modo toggle no pino PB1
    timer1.setOutputMode(Timer1::OutputMode::TOGGLE_ON_COMPARE, Timer1::OutputMode::NORMAL);

    setBit(DDRB, PB1);

    while(1) {

    }

    return 0;
}
