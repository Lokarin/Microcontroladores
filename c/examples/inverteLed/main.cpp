
#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define setBit(reg, pos)                reg |= (1 << pos)
#define cplBit(reg, pos)                reg ^= (1 << pos)
#define clrBit(reg, pos)                reg &= ~(1 << pos)

#define LED_A_DIR       DDRB
#define LED_A_OUT       PORTB
#define LED_A_PIN       PINB
#define LED_A_BIT       PB5

#define INT0_BTN_DIR       DDRD
#define INT0_BTN_OUT       PORTD
#define INT0_BTN_PIN       PIND
#define INT0_BTN_BIT       PD2


int main()
{

    setBit(LED_A_DIR, LED_A_BIT);       // LED as out

    clrBit(INT0_BTN_DIR, INT0_BTN_BIT);       // INT0 as in
    setBit(INT0_BTN_OUT, INT0_BTN_BIT);       // INT0 as in pull-up

    setBit(EICRA, ISC01);       // INT0 as falling edge
    clrBit(EICRA, ISC00);       // ...
    setBit(EIFR, INTF0);        // Clear interrupt flag (ignore old int. req.)
    setBit(EIMSK, INT0);        // INT0 Enabled


    sei();


    while(true) {
    }

    return 0;

}

ISR(INT0_vect)
{
    cplBit(LED_A_OUT, LED_A_BIT);
    _delay_ms(100);
    setBit(EIFR, INTF0);        // Clear interrupt flag
}
