#include <avr/io.h>
#include <util/delay.h>

#define setBit(reg, bit)                do{(reg) |= (1 << (bit));}while(false)
#define clrBit(reg, bit)                do{(reg) &= ~(1 << (bit));}while(false)
#define cplBit(reg, bit)                do{(reg) ^= (1 << (bit));}while(false)

#define isBitSet(reg, bit)              (bool)((reg) & (1 << (bit)))
#define isBitClr(reg, bit)              (bool)(!isBitSet(reg, bit))

#define delayMs(time)                   _delay_ms(time)

#define LED_REG_DIR                     DDRD
#define LED_REG_OUT                     PORTD

#define DATA_PIN                        PD0
#define CLOCK_PIN                       PD1
#define LATCH_PIN                       PD2

void pulse_pin(uint8_t pin)             // Funcao 'clock'
{
    setBit(PORTD, pin);                 //seta o pino em 1
    _delay_us(1);                   //espera 1us
    clrBit(PORTD, pin);                 // seta o pino em 0
}

void shift_out(uint16_t data)           // Funcao de envio dos dados
{
    for(int i = 15; i >= 0; i--) {      // para cada um dos 16 bits
        if(data & (1 << i)) {           // se data existir, o bit for 1, ele seta o pino como 1
            setBit(PORTD, DATA_PIN);
        } else {                        // se nao, ele limpa o pino
            clrBit(PORTD, DATA_PIN);
        }
        pulse_pin(CLOCK_PIN);       // geramos um pulso de clock para cada bit enviado
    }
}

void showFrame(uint16_t frame)          // funcao para motrar o frame carregado
{
    clrBit(PORTD, LATCH_PIN);           // primeiro desativamos os latchs
    shift_out(frame);             // enviamos o o frame
    pulse_pin(LATCH_PIN);          // e entao ativamos o latch, basicamente atualizando a saida
}

int main()
{
    // configurando saidas
    setBit(LED_REG_DIR, DATA_PIN);
    setBit(LED_REG_DIR, CLOCK_PIN);
    setBit(LED_REG_DIR, LATCH_PIN);

    // variaveis de controle
    uint16_t frame = 0x0001;
    uint8_t dir = 1;

    while(1) {
        // mostramos o frame e damos o delay de 250ms
        showFrame(frame);
        delayMs(250);

        if(dir) {
            frame <<= 1;                // deslocamos para a esquerda
            if(frame == 0x8000) {
                dir = 0;                // mudamos a direcao para o proximo loop se o bit já estiver no led mais da esquerda
            }
        } else {
            frame >>= 1;                // deslocamos para direita
            if(frame == 0x0001) {
                dir = 1;                // mudamos a direcao para o proximo loop se o bit já estiver no led mais da direta
            }
        }
    }

    return 0;
}
