#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibGpioBus.hpp"
#include "funsape/peripheral/funsapeLibPcint0.hpp"
#include "funsape/device/funsapeLibSevenSegmentsMuxDisplay.hpp"

uint8_t pinbAntigo = 0;
uint16_t valor = 0;
uint8_t digitos[4];

void criarDigitos(uint8_t lista[4], uint16_t valor)
{
    for(int i = 0; i < 4; i++) {
        digitos[i] = valor % 10;
        valor /= 10;
    }
}

int main()
{
    // variaveis
    SevenSegmentsMuxDisplay display;
    GpioBus controle;
    GpioBus data;

    // init de bus
    controle.init(&DDRC, GpioBus::PinIndex::P0, 4); data.init(&DDRD, GpioBus::PinIndex::P0, 8);

    // init display
    display.setPorts(&data, &controle, LogicLevel::LOW);
    display.init(SevenSegmentsMuxDisplay::Digits::DIGITS_4, SevenSegmentsDisplayType::COMMON_ANODE);

    // pcint0 init + interrupcoes
    pcint0.init(Pcint0::Pin::PIN_PCINT0 | Pcint0::Pin::PIN_PCINT1 | Pcint0::Pin::PIN_PCINT2 | Pcint0::Pin::PIN_PCINT3);
    pcint0.clearInterruptRequest();
    pcint0.activateInterrupt();

    // valor na tela e os digitos dele
    valor = 2048;
    criarDigitos(digitos, valor);

    pinbAntigo = PINB;

    sei();

    while(1) {
        delayMs(1);
        display.updateDigitValues(digitos);
        display.showNextDigit();
    }

    return 0;
}

//void pcint0InterruptCallback(void)
//{
//    uint8_t pinbAtual = PINB;
//    uint16_t over = 0;
//
//    // esse ^ é uma xor
//    uint8_t changedPins = pinbAtual ^ pinbAntigo;
//
//    if((changedPins & (1 << PB0)) && !(pinbAtual & (1 << PB0))) {
//        if ((valor + 1000) <= 10000) {
//            valor = valor + 1000;
//        } else {
//            over = (valor + 1000) - 10000;
//            valor = over;
//        }
//    }
//
//    if((changedPins & (1 << PB1)) && !(pinbAtual & (1 << PB1))) {
//        if ((valor + 100) <= 10000) {
//            valor = valor + 100;
//        } else {
//            over = (valor + 100) - 10000;
//            valor = over;
//        }
//    }
//
//    if((changedPins & (1 << PB2)) && !(pinbAtual & (1 << PB2))) {
//        if ((valor + 10) <= 10000) {
//            valor = valor + 10;
//        } else {
//            over = (valor + 10) - 10000;
//            valor = over;
//        }
//    }
//
//    if((changedPins & (1 << PB3)) && !(pinbAtual & (1 << PB3))) {
//        if ((valor + 1) <= 10000) {
//            valor = valor + 1;
//        } else {
//            over = (valor + 1) - 10000;
//            valor = over;
//        }
//    }
//
//    criarDigitos(digitos, valor);
//
//    // atualiza estado atual
//    pinbAntigo = pinbAtual;
//}

// Otimizacao feita pelo ClaudeAI

static uint16_t getIncremento(uint8_t pinIndex) {
    switch(pinIndex) {
        case 0: return 1000;  // PB0
        case 1: return 100;   // PB1
        case 2: return 10;    // PB2
        case 3: return 1;     // PB3
        default: return 0;
    }
}

void pcint0InterruptCallback(void)
{
    uint8_t pinbAtual = PINB;
    uint8_t changedPins = pinbAtual ^ pinbAntigo;
    
    for (uint8_t i = 0; i < 4; i++) {
        // i corresponde ao bit do pino: i=0 -> bit 0 (PB0), i=1 -> bit 1 (PB1), etc.
        if ((changedPins & (1 << i)) && !(pinbAtual & (1 << i))) {
            valor = (valor + getIncremento(i)) % 10000;
        }
    }
    
    criarDigitos(digitos, valor);
    pinbAntigo = pinbAtual;
}
