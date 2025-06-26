#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibAdc.hpp"
#include "funsape/peripheral/funsapeLibTimer0.hpp"
#include "funsape/peripheral/funsapeLibTimer1.hpp"

uint16_t rawAdcValue = 0;

int main()
{
    // Configure LED
    //setBit(DDRB, PB5);

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

    // Configure Timer1
    timer1.setInputCaptureValue(12500-1);
    timer1.setOutputMode(Timer1::OutputMode::NON_INVERTING_MODE, Timer1::OutputMode::NORMAL);
    timer1.init(Timer1::Mode::PWM_FAST_ICR, Timer1::ClockSource::PRESCALER_64);
    setBit(DDRB, PB1); // OC1A

    // Debugs
    setBit(DDRC, PC1);

    sei();

    while(true) {
        //cplBit(PORTB, PB5);
        //delayMs(100);
    }

    return 0;
}

void adcConversionCompleteCallback(void) {
    timer0.clearCompareAInterruptRequest();
    //cplBit(PORTC, PC1);

    rawAdcValue = ADC;

    uint16_t duty = (uint32_t)rawAdcValue * 12499 / 1023;
    timer1.setCompareAValue(duty);
}
