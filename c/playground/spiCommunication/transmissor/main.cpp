#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibAdc.hpp"
#include "funsape/peripheral/funsapeLibTimer0.hpp"

//vuint8_t spiSendBuffer[2];
//vuint8_t spiSendIndex = 0;
volatile bool_t spiBusy = false;

void spi_init_master() {
    // Pinos como saída
    setBit(DDRB, PB2);      // SS
    setBit(DDRB, PB3);      // MOSI
    setBit(DDRB, PB5);      // SCK
    clrBit(DDRB, PB4);      // MISO

    setBit(PORTB, PB2);     // SS em HIGH para garantir Master

    // SPI Enable, Master, Clock fosc/128
    setBit(SPCR, SPIE);     // Enable de interrupções, ativo em 1
    setBit(SPCR, SPE);      // Enable do SPI, ativo em 1
    clrBit(SPCR, DORD);     // Ordem dos bits. 1 para começar por MSB
    setBit(SPCR, MSTR);     // Master/Slave select. 1 para Master
    clrBit(SPCR, CPOL);     // Polariação do SCK. 1 para HIGH em idle
    setBit(SPCR, CPHA);     // Fase do clock. Em 1, Coloca o bit em MOSI na subida
    setBit(SPCR, SPR1);     // Bit de ClockRate 1
    setBit(SPCR, SPR0);     // Bit de ClockRate 0

    clrBit(SPSR, SPI2X);    // Double Speed. Ativo em 1.
}

void spi_send_byte(uint8_t data) {
    if (!spiBusy) {
        spiBusy = true;
        SPDR = data; // Envia byte
    }
    //while (isBitClr(SPSR, SPIF)); // Espera até a transmissão completar
}

int main()
{
    // Configure SPI
    spi_init_master();

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

    // SS do Slave
    setBit(DDRD, PD0);
    clrBit(PORTD, PD0);

    sei();

    while(true) {
    }

    return 0;
}

void adcConversionCompleteCallback(void) {
    timer0.clearCompareAInterruptRequest();
    //cplBit(PORTC, PC1);
    
    uint16_t adcValue = ADC; // Ex: 0b11_11111111

    // Enviando 16bits
    //spiSendBuffer[0] = (adcValue >> 8) & 0b00000011; // 0b000000XX
    //spiSendBuffer[1] = adcValue & 0xFF;              // 0bYYYYYYYY

    //spiSendIndex = 0;
    //spiBusy = true;
    //SPDR = spiSendBuffer[0];

    // Enviando 8bits de 16bits compressos
    uint8_t comprimido = adcValue >> 2;     // Divisão por 4
    spiBusy = true;
    SPDR = comprimido;
}

ISR(SPI_STC_vect) {
    // Enviando 16bits
    //spiSendIndex++;
    //if (spiSendIndex < 2) {
    //    SPDR =spiSendBuffer[spiSendIndex];
    //} else {
    //    spiBusy = false;
    //}

    // Enviando 8bits
    spiBusy = false;
}
