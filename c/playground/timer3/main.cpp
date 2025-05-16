#include <avr/io.h>
#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibTimer0.hpp"

int main()
{
    // Configura saida
    setBit(DDRC, PC0);
    setBit(DDRC, PC1);
    setBit(DDRC, PC2);

    // Inicia timer em modo Clear Time on Compare Match, com prescalar 64
    timer0.init(Timer0::Mode::CTC_OCRA, Timer0::ClockSource::PRESCALER_64);

    // Armazenamos os valores de OCR0A e OCR0B
    timer0.setCompareAValue(124);
    timer0.setCompareBValue(100);

    // Habilitamos as interrupcoes
    timer0.setCompareAValue(124);
    timer0.setCompareBValue(100);
    timer0.activateCompareAInterrupt();
    timer0.activateCompareBInterrupt();
    timer0.activateOverflowInterrupt();
    timer0.clearCompareAInterruptRequest();
    timer0.clearCompareBInterruptRequest();
    timer0.clearOverflowInterruptRequest();

    // Output dos pinos de timer0
    timer0.setOutputMode(
            Timer0::OutputMode::TOGGLE_ON_COMPARE,
            Timer0::OutputMode::TOGGLE_ON_COMPARE
    );
    setBit(DDRD, PD6);
    setBit(DDRD, PD5);

    // Habilitamos interrupcoes globais
    sei();

    while(1) {

    }

    return 0;
}

void timer0CompareACallback(void)
{
    cplBit(PORTC, PC0);
}

void timer0CompareBCallback(void)
{
    cplBit(PORTC, PC1);
}

void timer0OverflowCallback(void)
{
    cplBit(PORTC, PC2);
}
