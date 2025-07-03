/*
 * Project:  Digipot Control
 * File:     main.cpp
 * Author:   Gabriel Garcia; Henrique Onuki
 * Created:  2025-04-19
 * Modified: 2025-06-15 (Converted to C++)
 * Version:  2.0
 * Notes:    Controle de Digipots. Fcpu = 16 MHz.
 *           Converted from AVR Assembly to C++
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// Constants
#define TRIGGER_SECTION        0x00
#define TRIGGER_AUTO           0x00
#define TRIGGER_MANUAL_AND_OFF 0x01
#define TRIGGER_MANUAL_AND_ON  0x03
#define TWBR_BIT_RATE          18
#define SLA0_W                 0b01010000
#define SLA1_W                 0b01010010
#define WR_POT_0               0b10101001

// Global variables
volatile uint8_t timer1FreqByteLow = 0;
volatile uint8_t timer1FreqByteHigh = 0;
volatile uint8_t triggerByte = 1;
volatile uint8_t attackByte = 2;
volatile uint8_t decayAndReleaseByte = 4;
volatile uint8_t holdByte = 8;
volatile uint8_t sustainByte = 16;

// Function prototypes
void initTimer1();
void initUSART();
void initTWI();
void initPCINT2();
void initINT1();
void jackDetectorInput();
void activateInternalTrigger();
void activateExternalTrigger();
void setTrigger(uint8_t triggerMode);
void setTriggerAuto();
void setTriggerManualOn();
void setTriggerManualOff();
uint8_t usartReceiveByte();
void twiWaitForCompletion();
void updateDigipots();

// Timer1 initialization for ADSR trigger
void initTimer1()
{
    // PB1 as output
    DDRB |= (1 << PB1);

    // Initialize OCR1A with value equivalent to 3Hz
    OCR1A = 10416;

    triggerByte = 1; // Manual trigger and off

    // Check which trigger should be chosen
    jackDetectorInput();
}

// USART initialization at 9600 bps
void initUSART()
{
    // Baud rate: 9600 @ 16MHz
    UBRR0 = 103;

    // 8 bits, 1 stop, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

    // Enable RX pin (PD0) and interrupt
    UCSR0B = (1 << RXCIE0) | (1 << RXEN0);
}

// TWI (I2C) initialization
void initTWI()
{
    // SCL freq = 100kHz
    TWBR = TWBR_BIT_RATE;

    // Prescaler = 1
    TWSR = (0 << TWPS1) | (0 << TWPS0);

    // Enable TWI
    TWCR = (1 << TWEN);
}

// PCINT2 configuration
void initPCINT2()
{
    // Enable interrupt group (PCINT16-PCINT23)
    PCICR = (1 << PCIE2);

    // Enable interrupt on PCINT20
    PCMSK2 = (1 << PCINT20);

    // Enable PD4 as input with internal pull-up
    DDRD &= ~(1 << PD4);
    PORTD |= (1 << PD4);
}

// INT1 configuration
void initINT1()
{
    // Enable int1 pin as input
    DDRD &= ~(1 << PD3);
    PORTD |= (1 << PD3); // Internal pull-up enabled

    // Detect any change on pin
    EICRA = (1 << ISC10);
}

// Jack detector input - switches between triggers
void jackDetectorInput()
{
    if(!(PIND & (1 << PD4))) {
        activateExternalTrigger();
    } else {
        activateInternalTrigger();
    }
}

void activateInternalTrigger()
{
    // Configure current trigger command
    setTrigger(triggerByte);

    // Disable INT1 interrupt
    EIMSK &= ~(1 << INT1);

    // Prescaler 256 + CTC mode
    TCCR1B = (1 << CS12) | (1 << WGM12);
}

void activateExternalTrigger()
{
    // Disable Timer1
    TCCR1B = 0;
    TCCR1A = 0;

    // Enable INT1 interrupt
    EIMSK |= (1 << INT1);
}

// Configure trigger mode
void setTrigger(uint8_t triggerMode)
{
    switch(triggerMode) {
    case TRIGGER_AUTO:
        setTriggerAuto();
        break;
    case TRIGGER_MANUAL_AND_ON:
        setTriggerManualOn();
        break;
    case TRIGGER_MANUAL_AND_OFF:
        setTriggerManualOff();
        break;
    }
}

// Configure trigger for auto mode
void setTriggerAuto()
{
    // OC1A (PB1) as toggle
    TCCR1A = (1 << COM1A0);
}

// Configure trigger for manual mode ON
void setTriggerManualOn()
{
    // OC1A (PB1) toggle disabled
    TCCR1A = (0 << COM1A0);

    // Activate PB1 (HIGH level)
    PORTB |= (1 << PB1);
}

// Configure trigger for manual mode OFF
void setTriggerManualOff()
{
    // OC1A (PB1) toggle disabled
    TCCR1A = (0 << COM1A0);

    // Deactivate PB1 (LOW level)
    PORTB &= ~(1 << PB1);
}

// Receive byte via USART
uint8_t usartReceiveByte()
{
    while(!(UCSR0A & (1 << RXC0)));  // Wait for data
    return UDR0;
}

// Wait for TWI completion
void twiWaitForCompletion()
{
    while(!(TWCR & (1 << TWINT)));
}

// Update digipots via TWI
void updateDigipots()
{
    // Start condition
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    twiWaitForCompletion();

    // Send SLA0_W (device 0 address + write)
    TWDR = SLA0_W;
    TWCR = (1 << TWINT) | (1 << TWEN);
    twiWaitForCompletion();

    // Send command byte
    TWDR = WR_POT_0;
    TWCR = (1 << TWINT) | (1 << TWEN);
    twiWaitForCompletion();

    // Send attack byte for pot0
    TWDR = attackByte;
    TWCR = (1 << TWINT) | (1 << TWEN);
    twiWaitForCompletion();

    // Send decay/release byte for pot1
    TWDR = decayAndReleaseByte;
    TWCR = (1 << TWINT) | (1 << TWEN);
    twiWaitForCompletion();

    // Repeated start
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    twiWaitForCompletion();

    // Send SLA1_W (device 1 address + write)
    TWDR = SLA1_W;
    TWCR = (1 << TWINT) | (1 << TWEN);
    twiWaitForCompletion();

    // Send command byte
    TWDR = WR_POT_0;
    TWCR = (1 << TWINT) | (1 << TWEN);
    twiWaitForCompletion();

    // Send hold byte for pot0
    TWDR = holdByte;
    TWCR = (1 << TWINT) | (1 << TWEN);
    twiWaitForCompletion();

    // Send sustain byte for pot1
    TWDR = sustainByte;
    TWCR = (1 << TWINT) | (1 << TWEN);
    twiWaitForCompletion();

    // Stop condition
    TWCR = (1 << TWSTO) | (1 << TWINT) | (1 << TWEN);
}

// Interrupt Service Routines

// INT1 interrupt handler
ISR(INT1_vect)
{
    if(PIND & (1 << PD3)) {
        // Activate trigger
        PORTB |= (1 << PB1);
    } else {
        // Deactivate trigger
        PORTB &= ~(1 << PB1);
    }
}

// PCINT2 interrupt handler
ISR(PCINT2_vect)
{
    jackDetectorInput();
}

// USART RX interrupt handler
ISR(USART_RX_vect)
{
    uint8_t firstByte = usartReceiveByte();

    if(firstByte == TRIGGER_SECTION) {
        // Trigger section
        uint8_t secondByte = usartReceiveByte();
        triggerByte = secondByte;
        setTrigger(triggerByte);
    } else {
        // ADSR section
        attackByte = usartReceiveByte();
        holdByte = usartReceiveByte();
        sustainByte = usartReceiveByte();
        decayAndReleaseByte = usartReceiveByte();
        timer1FreqByteHigh = usartReceiveByte();
        timer1FreqByteLow = usartReceiveByte();

        // Set ADSR trigger frequency
        OCR1A = (timer1FreqByteHigh << 8) | timer1FreqByteLow;

        // Update digipots
        updateDigipots();
    }
}

// Flash handler (placeholder)
ISR(SPM_READY_vect)
{
    // Flash operations if needed
}

// Main function
int main()
{
    // Disable USART during initialization
    UCSR0B = 0;

    // Initialize system
    initTimer1();
    initUSART();
    initTWI();
    initPCINT2();
    initINT1();

    // Enable global interrupts
    sei();

    // Main loop - wait for interrupts
    while(1) {
        // Main processing happens in interrupt handlers
        // Add any background tasks here if needed
        _delay_ms(1000);
        updateDigipots();
    }

    return 0;
}

