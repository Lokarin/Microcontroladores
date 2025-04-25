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

uint8_t hexToSevenSegmentsCommonAnode(uint8_t hex_p);

int main()
{
    uint8_t counter = 0;

    DDRD = 0xFF;

    while(1) {
        PORTD = hexToSevenSegmentsCommonAnode(counter);
        counter = (counter == 9) ? (0) : (counter + 1);

        delayMs(500);
    }

    return 0;
}

uint8_t hexToSevenSegmentsCommonAnode(uint8_t hex_p)
{
    uint8_t auxSegments = 0;

    switch(hex_p) {  //           0bPGFEDCBA
    case 0x00:      auxSegments = 0b11000000;   break;
    case 0x01:      auxSegments = 0b11111001;   break;
    case 0x02:      auxSegments = 0b10100100;   break;
    case 0x03:      auxSegments = 0b10110000;   break;
    case 0x04:      auxSegments = 0b10011001;   break;
    case 0x05:      auxSegments = 0b10010010;   break;
    case 0x06:      auxSegments = 0b10000010;   break;
    case 0x07:      auxSegments = 0b11111000;   break;
    case 0x08:      auxSegments = 0b10000000;   break;
    case 0x09:      auxSegments = 0b10010000;   break;
    case 0x0A:      auxSegments = 0b10001000;   break;
    case 0x0B:      auxSegments = 0b10000011;   break;
    case 0x0C:      auxSegments = 0b11000110;   break;
    case 0x0D:      auxSegments = 0b10100001;   break;
    case 0x0E:      auxSegments = 0b10000110;   break;
    case 0x0F:      auxSegments = 0b10001110;   break;
    default:        auxSegments = 0b11111111;   break;
    }

    return auxSegments;
}
