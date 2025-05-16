#include <avr/io.h>
#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibTimer0.hpp"

int main()
{
    // Configura saida
    setBit(DDRC, PC0);

    // Inicia timer em modo Clear Time on Compare Match, com prescalar 64
    timer0.init(Timer0::Mode::CTC_OCRA, Timer0::ClockSource::PRESCALER_64);

    // Armazenamos o valor de 249 como valor de comparacao na OCR0A
    timer0.setCompareAValue(249);

    timer0.activateCompareAInterrupt();

    sei();

    while(1) {

    }

    return 0;
}

void timer0CompareACallback(void)
{
    cplBit(PORTC, PC0);
}
