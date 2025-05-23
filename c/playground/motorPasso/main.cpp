#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibTimer1.hpp"
#include "funsape/peripheral/funsapeLibPcint0.hpp"


uint8_t pinbAntigo = 0;

volatile uint8_t velocidade = 0;
volatile uint8_t faseAtual = 0;

volatile bool_t ligado = 1;
volatile bool_t sentido = 0;

const uint16_t atrasos[4] = {6249, 3124, 1562, 780};
const uint8_t fases[4] = {
    (1 << PD0),
    (1 << PD1),
    (1 << PD2),
    (1 << PD3)
};


// funcao pra aumentar a velocidade. Ou voltar pra menor.
void atualizaTimer()
{
    timer1.setCounterValue(0);
    timer1.setCompareAValue(atrasos[velocidade]);
}

// deixa o motor desligado
void desligarFases()
{
    clrBit(PORTD, PD0);
    clrBit(PORTD, PD1);
    clrBit(PORTD, PD2);
    clrBit(PORTD, PD3);
}

int main()
{
    // Set saidas do motor
    setBit(DDRD, PD0);
    setBit(DDRD, PD1);
    setBit(DDRD, PD2);
    setBit(DDRD, PD3);

    // seta todas as saidas do motor em baixo
    desligarFases();

    // setando como entrada pull-up
    clrBit(DDRB, PB2);
    clrBit(DDRB, PB3);
    clrBit(DDRB, PB4);

    setBit(PORTB, PB2);
    setBit(PORTB, PB3);
    setBit(PORTB, PB4);

    // habilitando int2,3,4
    setBit(PCICR, PCIE0);
    setBit(PCMSK0, PCINT2);
    setBit(PCMSK0, PCINT3);
    setBit(PCMSK0, PCINT4);

    // armazenando PINB atual
    pinbAntigo = PINB;

    setBit(DDRC, PC0);
    setBit(DDRC, PC1);

    // iniciando timer em modo ctc ocra
    timer1.init(Timer1::Mode::CTC_OCRA, Timer1::ClockSource::PRESCALER_1024);

    // setando valor de comparacao
    atualizaTimer();

    // habilitando interrupcao de comparacao com A
    timer1.activateCompareAInterrupt();
    timer1.clearCompareAInterruptRequest();

    // habilitando interrupcoes globais
    sei();

    while(1) {
    }

    return 0;
}

void timer1CompareACallback(void)
{
    cplBit(PORTC, PC0);

    if(ligado) {
        // zera PB0-PB3 sem mexer nos outros quatro
        // ~ inverte os bits, entao na pratica a "mascara" é 0b11110000
        // o operador &= é uma and. Ela vai manter 1 somente onde PORTB
        // e a mascara sao. Na pratica, estamos zerando os primeiros 4 bits
        // sem afetar os ultimos 4
        PORTD &= ~0b00001111;

        // esse |= é uma ou. como a mascara zerou os ultimos 4 logo em cima,
        // na pratica, isso da load na proxima fase, sem mexer em PB4 pra cima.
        PORTD |= fases[faseAtual];

        if(sentido) {
            faseAtual = (faseAtual + 1) % 4;
        } else {
            faseAtual = (faseAtual + 3) % 4;
        }

    } else {
        desligarFases();
    }
}

void pcint0InterruptCallback()
{
    uint8_t pinbAtual = PINB;

    // esse ^ é uma xor
    uint8_t changedPins = pinbAtual ^ pinbAntigo;

    if((changedPins & (1 << PB2)) && !(pinbAtual & (1 << PB2))) {
        ligado = !ligado;
        cplBit(PORTC, PC1);
    }

    if((changedPins & (1 << PB3)) && !(pinbAtual & (1 << PB3))) {
        sentido = !sentido;
        cplBit(PORTC, PC1);
    }

    if((changedPins & (1 << PB4)) && !(pinbAtual & (1 << PB4))) {
        if(velocidade < 3) {
            velocidade += 1;
            atualizaTimer();
            cplBit(PORTC, PC1);
        } else {
            velocidade = 0;
            atualizaTimer();
            cplBit(PORTC, PC1);

        }
    }

    // atualiza estado atual
    pinbAntigo = pinbAtual;
}
