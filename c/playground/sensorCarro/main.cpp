#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibGpioBus.hpp"
#include "funsape/device/funsapeLibSevenSegmentsMuxDisplay.hpp"
#include "funsape/peripheral/funsapeLibTimer0.hpp"
#include "funsape/peripheral/funsapeLibTimer1.hpp"
#include "funsape/peripheral/funsapeLibTimer2.hpp"
#include "funsape/peripheral/funsapeLibAdc.hpp"
#include "funsape/device/funsapeLibSevenSegmentsDisplay.hpp"

// variaveis globais
SevenSegmentsMuxDisplay display;
uint8_t digitos[4] = {8, 8, 8, 8};
bool pontos[4] = {false, false, false, false};
vuint16_t tanque = 0;
vuint16_t temp = 0;
bool_t lendoLM35 = false;
volatile bool mostrandoTemp = false;
volatile bool primeiraInterrupcao = true;

int main()
{
    // variaveis
    GpioBus controle;
    GpioBus data;

    // init de bus
    controle.init(&DDRB, GpioBus::PinIndex::P2, 4); data.init(&DDRD, GpioBus::PinIndex::P0, 8);

    // init display
    display.setPorts(&data, &controle, LogicLevel::LOW);
    display.init(SevenSegmentsMuxDisplay::Digits::DIGITS_4, SevenSegmentsDisplayType::COMMON_ANODE);

    // timer0
    timer0.setCompareAValue(156-1);
    timer0.init(Timer0::Mode::CTC_OCRA, Timer0::ClockSource::PRESCALER_1024);
    timer0.clearCompareAInterruptRequest();
    timer0.clearCompareBInterruptRequest();

    // timer1
    timer1.setCompareAValue(15625-1);
    timer1.init(Timer1::Mode::CTC_OCRA, Timer1::ClockSource::PRESCALER_1024);
    timer1.clearCompareAInterruptRequest();
    timer1.activateCompareAInterrupt();

    // timer2
    timer2.setCompareAValue(78-1);
    timer2.init(Timer2::Mode::CTC_OCRA, Timer2::ClockSource::PRESCALER_1024);
    timer2.clearCompareAInterruptRequest();
    timer2.activateCompareAInterrupt();

    // adc
    adc.init(
            Adc::Mode::AUTO_TIMER0_COMPA, 
            Adc::Reference::POWER_SUPPLY, 
            Adc::Prescaler::PRESCALER_128
    );
    adc.setChannel(Adc::Channel::CHANNEL_0);
    adc.clearInterruptRequest();
    adc.activateInterrupt();
    adc.enable();

    // coisas para debug
    setBit(DDRB, PB0);
    setBit(DDRB, PB1);

    // LEDs
    setBit(DDRC, PC2);
    setBit(DDRC, PC3);

    display.updateDigitValues(digitos);
    for (uint16_t i = 0; i < 600; ++i) { 
        display.showNextDigit();
        delayMs(5);
    }

    sei();

    while(1) {
    }

    return 0;
}

void adcConversionCompleteCallback(void) {
    // logica pare ler adc0 ou 1 
    if (!lendoLM35) {
        setBit(PORTB, PB0);
        tanque = ADC;

        lendoLM35 = true;
        timer0.clearCompareAInterruptRequest();

        adc.setMode(Adc::Mode::SINGLE_CONVERSION);
        adc.setChannel(Adc::Channel::CHANNEL_1);
        adc.setReference(Adc::Reference::INTERNAL);

        adc.startConversion();
    } else {
        clrBit(PORTB, PB0);
        temp = ADC;

        lendoLM35 = false;
        timer0.clearCompareAInterruptRequest();
        
        adc.setMode(Adc::Mode::AUTO_TIMER0_COMPA);
        adc.setChannel(Adc::Channel::CHANNEL_0);
        adc.setReference(Adc::Reference::POWER_SUPPLY);
    }
}

void timer1CompareACallback(void) {
    timer1.clearCompareAInterruptRequest();

    if (primeiraInterrupcao) {
        primeiraInterrupcao = false;
        return;
    }

    mostrandoTemp = !mostrandoTemp;

    // led vermelho para temperatura acima de 100C
    if (temp >= ((uint32_t)1023 * 100) / 110) {
        setBit(PORTC, PC2);  
    } else {
        clrBit(PORTC, PC2);  
    }

    // calculando litros para o led amarelo
    uint16_t litros = ((uint32_t)tanque * 500) / 1023;
    static bool piscaAmarelo = false;

    // liga ou pisca o led dependendo se estiver
    // abaixo de 3 ou 7L
    if (litros <= 30) { 
        piscaAmarelo = !piscaAmarelo;
        if (piscaAmarelo)
            setBit(PORTC, PC3);  
        else
            clrBit(PORTC, PC3);  
    }
    else if (litros <= 70) { 
        setBit(PORTC, PC3); 
    } else {
        clrBit(PORTC, PC3);
    }
}


void timer2CompareACallback(void) {
    timer2.clearCompareAInterruptRequest();
    cplBit(PORTB, PB1);
    
    // escolhendo entre temperatura ou litros
    if (!mostrandoTemp) {
        uint16_t litros = ((uint32_t)tanque * 500) / 1023;
        
        digitos[1] = litros % 10;
        digitos[2] = (litros / 10) % 10;
        digitos[3] = (litros / 100) % 10;
        digitos[0] = (uint8_t)SevenSegmentsCode::LETTER_L;
        
        pontos[0] = pontos[1] = pontos[3] = false;
        pontos[2] = (tanque > 0);
        
    } else {
        uint8_t celsius = ((uint32_t)temp * 110) / 1023;
        
        digitos[3] = celsius / 100;
        digitos[2] = (celsius / 10) % 10;
        digitos[1] = celsius % 10;
        digitos[0] = 12;  
        
        pontos[0] = pontos[1] = pontos[2] = pontos[3] = false;
    }
    
    display.updateDigitValues(digitos, pontos);
    display.showNextDigit();
}
