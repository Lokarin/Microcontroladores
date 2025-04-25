#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#define setBit(reg, bit)                do{(reg) |= (1 << (bit));}while(false)
#define clrBit(reg, bit)                do{(reg) &= ~(1 << (bit));}while(false)
#define cplBit(reg, bit)                do{(reg) ^= (1 << (bit));}while(false)

#define isBitSet(reg, bit)              (bool)((reg) & (1 << (bit)))
#define isBitClr(reg, bit)              (bool)(!isBitSet(reg, bit))

#define delayMs(time)                   _delay_ms(time)

#define LED_REG_DIR                     DDRB
#define LED_REG_OUT                     PORTB
#define LED_REG_BIT                     PB5

int main()
{
    setBit(LED_REG_DIR, LED_REG_BIT);

    while(1) {
        setBit(LED_REG_OUT, LED_REG_BIT);
        delayMs(1000);
        clrBit(LED_REG_OUT, LED_REG_BIT);
        delayMs(1000);
    }

    return 0;
}
