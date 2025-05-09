#define F_CPU 16000000UL

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
#define LED_REG_BIT                     PB5


int main() {
    LED_REG_DIR = 0xFF;     
    LED_REG_OUT = 0x00;     

    uint8_t pushed = 0x02;      // Começa no LED 1

    while (1) {
        uint8_t pusher = 0x01;  // Começa no LED 0

        while (pusher < pushed) {
            LED_REG_OUT = pusher | pushed;
            delayMs(250);
            pusher <<= 1;
        }

        LED_REG_OUT = pushed;
        delayMs(250);

        pushed <<= 1;

        if (pushed == 0x00) {
            pushed = 0x02;
        }
    }

    return 0;
}
