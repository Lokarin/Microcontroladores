#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibAdc.hpp"
#include "funsape/peripheral/funsapeLibTimer0.hpp"
#include "spiAtmega328p/spiAtmega328p.hpp"

int main()
{
    // Configure SPI
    Spi::init(Spi::Mode::SLAVE, Spi::ClockRate::FOSC_4);
    Spi::activateSpiCallbackInterrupt();

    // Configure ADC
    adc.init(
            Adc::Mode::AUTO_TIMER0_COMPA, 
            Adc::Reference::INTERNAL, 
            Adc::Prescaler::PRESCALER_128
    );
    adc.setChannel(Adc::Channel::CHANNEL_0);
    adc.clearInterruptRequest();
    adc.activateInterrupt();
    adc.enable();

    // Configure Timer0
    timer0.setCompareAValue(156-1);
    timer0.init(Timer0::Mode::CTC_OCRA, Timer0::ClockSource::PRESCALER_1024);
    timer0.clearCompareAInterruptRequest();

    // Debug
    setBit(DDRC, PC1);

    // Pino de inicio de comunicação
    setBit(DDRD, PD0);
    setBit(PORTD, PD0);

    sei();

    while(true) {
    }

    return 0;
}

void adcConversionCompleteCallback(void) {
    timer0.clearCompareAInterruptRequest();
    //cplBit(PORTC, PC1);
    
    uint16_t adcValue = ADC; // Ex: 0b11_11111111

    // Enviando 8bits de 16bits compressos
    SPDR = adcValue >> 2;     // Divisão por 4

    // Avisar o Master
    clrBit(PORTD, PD0);
}

void Spi::spiCallbackInterrupt(uint8_t received) {
    setBit(PORTD, PD0);
}
