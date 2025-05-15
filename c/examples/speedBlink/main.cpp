#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define inlined                             inline __attribute__((always_inline))
#define setBit(reg, pos)                    do{(reg) |= (1 << (pos));}while(0)
#define clrBit(reg, pos)                    do{(reg) &= ~(1 << (pos));}while(0)
#define cplBit(reg, pos)                    do{(reg) ^= (1 << (pos));}while(0)
#define isBitSet(reg, pos)                  (((reg) >> (pos)) & 1)
#define isBitClr(reg, pos)                  (!isBitSet(reg,pos))

#define waitUntilBitIsSet(reg, pos)          do{     \
        doNothing();                                    \
    } while (isBitClr((reg), (pos)))

#define waitUntilBitIsClear(reg, pos)        do{     \
        doNothing();                                    \
    }while(isBitSet((reg), (pos)))

#   define setMask(reg, mask)                   do{(reg) |= (mask);}while(0)
#   define clrMask(reg, mask)                   do{(reg) &= ~(mask);}while(0)
#   define cplMask(reg, mask)                   do{(reg) ^= (mask);}while(0)
#   define setMaskOffset(reg, mask, offset)     do{(reg) |= ((mask) << (offset));}while(0)
#   define clrMaskOffset(reg, mask, offset)     do{(reg) &= ~((mask) << (offset));}while(0)
#   define cplMaskOffset(reg, mask, offset)     do{(reg) ^= ((mask) << (offset));}while(0)

void inlined doNothing(void)
{
    return;
}

volatile uint8_t blinkSpeed = 1;

int main()
{
    setBit(DDRB, PB5);

    setBit(DDRD, PD2);
    setBit(EICRA, ISC01);
    clrBit(EICRA, ISC00);
    setBit(EIFR, INTF0);
    setBit(EIMSK, INT0);

    setBit(DDRD, PD3);
    setBit(EICRA, ISC11);
    clrBit(EICRA, ISC10);
    setBit(EIFR, INTF1);
    setBit(EIMSK, INT1);

    sei();

    while(true) {
        switch(blinkSpeed) {
        case 3:
            _delay_ms(100);
        case 2:
            _delay_ms(100);
        case 1:
            _delay_ms(100);
            cplBit(PORTB, PB5);
        case 0:
        default:
            break;
        }

    }

    return 0;
}

ISR(INT0_vect)
{
    blinkSpeed = 0;
}

ISR(INT1_vect)
{
    blinkSpeed = (blinkSpeed == 3) ? 3 : (blinkSpeed + 1);
}
