#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>

uint8_t teste = 0;

void sendByte(uint8_t data) {
    while (!(UCSR0A & (1 << UDRE0))); // Espera o registrador estar livre
    UDR0 = data;
}

int main() {
    // Configurar USART
    UBRR0 = 103; // 9600 bps @ 16MHz
    UCSR0B = (1 << TXEN0); // Habilita transmissão
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // 8N1

    _delay_ms(1000); // Aguarda receptor inicializar
    
    if (teste == 0) {
        // Setar o Trigger em automatico
        sendByte(0x00); // TRIGGER_SECTION
        sendByte(0x00); // TRIGGER_MANUAL_AND_ON

        _delay_ms(2000);

        // Valores de AHDSR e Freq de Trigger
        sendByte(0x01); // Qualquer valor exceto 0x00 para indicar seção ADSR
        sendByte(0x02); // attackByte
        sendByte(0x08); // holdByte
        sendByte(0x10); // sustainByte
        sendByte(0x04); // decayAndReleaseByte
        sendByte(0x10); // timer1FreqByteHigh
        sendByte(0x00); // timer1FreqByteLow

    } else {
        // Set trigger em alto
        sendByte(0x00); // TRIGGER_SECTION
        sendByte(0x03); // TRIGGER_MANUAL_AND_ON

        _delay_ms(500);

        // Set trigger em baixo
        sendByte(0x00); // TRIGGER_SECTION
        sendByte(0x01); // TRIGGER_MANUAL_AND_OFF

        _delay_ms(500);

        // Setar o Trigger em automatico
        sendByte(0x00); // TRIGGER_SECTION
        sendByte(0x00); // TRIGGER_MANUAL_AND_ON
    }

    while (1) {
        // Nada
    }
}

