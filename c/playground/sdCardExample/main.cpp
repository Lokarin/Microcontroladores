// =============================================================================
// Project:         SD Card interface example
// File:            main.cpp
// Author:          Leandro Schwarz
// Created:         2025-06-25
// Modified:        2025-06-25
// Version:         1.0
// Notes:           This project is an example of how to integrate the FatFS
//                      Library with the FunSAPE AVR Library.
// Purpose:         The system mounts an SD Card, searches for all the files
//                      named with the "FILEXXXX.TXT" pattern and creates a new
//                      file in order to store the value of the temperature of
//                      a room, sampled at each second by the ADC and stored
//                      into the SD card. To debug the system, a LED and the
//                      USART0 module are used.
// =============================================================================

// =============================================================================
// Precompiler constant defintions
// =============================================================================

#define F_CPU 16000000UL

// =============================================================================
// Dependencies
// =============================================================================

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibAdc.hpp"
#include "funsape/peripheral/funsapeLibTimer0.hpp"
#include "funsape/peripheral/funsapeLibTimer1.hpp"
#include "funsape/peripheral/funsapeLibUsart0.hpp"
#include "fatFs/ff.h"
#include "fatFs/mmc_avr.h"

// =============================================================================
// Constant definitions
// =============================================================================

// NONE

// =============================================================================
// New data types
// =============================================================================

typedef union {
    struct {
        bool_t acquisitionCompleted     : 1;
        uint16_t rawTemperature         : 10;
        uint16_t unusedFlags            : 5;
    };
    uint16_t allFlags;
} systemFlags_t;

// =============================================================================
// Static function declarations
// =============================================================================

static inlined void ledTurnOn(void);
static inlined void ledTurnOff(void);
static inlined void ledToggle(void);

// =============================================================================
// Global variables
// =============================================================================

volatile systemFlags_t systemFlags;

// =============================================================================
// Main function
// =============================================================================

int main()
{
    // =========================================================================
    // Variable declaration
    // =========================================================================

    // -------------------------------------------------------------------------
    // Auxilary variables ------------------------------------------------------

    uint16_t aux16 = 0;
    char_t strBuffer[50];

    // -------------------------------------------------------------------------
    // SD Card variables -------------------------------------------------------

    FRESULT sdCardResult = FR_OK;       // Result of last function
    FATFS sdCardVolume;                 // Card volume handler
    char_t sdCardFileName[14];          // String with the file name
    FIL sdCardFile;                     // File handler

    // =========================================================================
    // Variable initialization
    // =========================================================================

    systemFlags.allFlags = 0;

    // =========================================================================
    // LED configuration
    // =========================================================================

    setBit(DDRD, PD7);                  // LED as output
    ledTurnOff();
    setBit(DDRD, PD3);
    setBit(DDRD, PD4);
    setBit(DDRD, PD5);
    setBit(DDRD, PD6);

    // =========================================================================
    // USART0 configuration
    // =========================================================================

    usart0.setBaudRate(Usart0::BaudRate::BAUD_RATE_57600);
    usart0.setFrameFormat(Usart0::FrameFormat::FRAME_FORMAT_8_N_1);
    usart0.init();
    usart0.enableTransmitter();
    usart0.stdio();
    printf("[  OK  ] USART0 configured!\r");

    // =========================================================================
    // Enable global interrupts
    // =========================================================================

    sei();

    // =========================================================================
    // TIMER0 configuration
    // =========================================================================

    timer0.setCompareAValue(155);
    timer0.init(Timer0::Mode::CTC_OCRA, Timer0::ClockSource::PRESCALER_1024);
    timer0.clearCompareAInterruptRequest();
    timer0.activateCompareAInterrupt();
    printf("[  OK  ] TIMER0 configured!\r");

    // =========================================================================
    // SD CARD configuration
    // =========================================================================

    // -------------------------------------------------------------------------
    // SD card - Mount the card ------------------------------------------------

    sdCardCreate(LOCATION_PB2);
    sdCardResult = f_mount(&sdCardVolume, "", 1);
    if(sdCardResult != FR_OK) {
        printf("[ FAIL ] SD card mount failed with error %u!\r", sdCardResult);
        systemHalt();
    }
    printf("[  OK  ] SD card mounted!\r");

    // -------------------------------------------------------------------------
    // SD card - Create a new file ---------------------------------------------

    for(aux16 = 1; aux16 < 10000; aux16++) {
        sprintf(sdCardFileName, "FILE%04u.TXT", aux16);
        sdCardResult = f_open(&sdCardFile, sdCardFileName, FA_CREATE_NEW | FA_WRITE);
        if(sdCardResult == FR_OK) {
            break;
        }
    }
    printf("[ INFO ] File '%s' created.\r", sdCardFileName);



    // =========================================================================
    // ADC configuration
    // =========================================================================

    adc.setChannel(Adc::Channel::CHANNEL_0);
    adc.init(Adc::Mode::AUTO_TIMER1_COMPB, Adc::Reference::INTERNAL, Adc::Prescaler::PRESCALER_128);
    adc.enable();
    adc.disableDigitalInput(Adc::DigitalInput::INPUT_0);
    adc.clearInterruptRequest();
    adc.activateInterrupt();
    printf("[  OK  ] ADC configured!\r");

    // =========================================================================
    // TIMER1 configuration
    // =========================================================================

    timer1.setCompareAValue(62500);
    timer1.setCompareBValue(62500);
    timer1.init(Timer1::Mode::CTC_OCRA, Timer1::ClockSource::PRESCALER_256);
    timer1.clearCompareAInterruptRequest();
    timer1.activateCompareAInterrupt();
    printf("[  OK  ] TIMER1 configured!\r");

    // =========================================================================
    // Main loop
    // =========================================================================

    while(true) {


        if(systemFlags.acquisitionCompleted) {

            sprintf(strBuffer, "ADC=%04u\r\n", systemFlags.rawTemperature);
            printf(strBuffer);

            setBit(PORTD, PD6);
            sdCardResult = f_write(&sdCardFile, strBuffer, strlen(strBuffer), (UINT *)&aux16);
            clrBit(PORTD, PD6);
            if(sdCardResult != FR_OK) {
                printf("[ FAIL ] Could not write data. Error=%u\r", sdCardResult);
                systemHalt();
            }
            printf("[  OK  ] Data written. %u bytes!\r", aux16);

            setBit(PORTD, PD6);
            sdCardResult = f_sync(&sdCardFile);
            clrBit(PORTD, PD6);
            if(sdCardResult != FR_OK) {
                printf("[ FAIL ] Could not save file. Error=%u\r", sdCardResult);
                systemHalt();
            }

            printf("[  OK  ] File saved.\r");
            ledToggle();
            systemFlags.acquisitionCompleted = false;
        }
    }

    return 0;
}

// =============================================================================
// Static function definition
// =============================================================================

static inlined void ledTurnOn(void)
{
    setBit(PORTD, PD7);
}

static inlined void ledTurnOff(void)
{
    clrBit(PORTD, PD7);
}

static inlined void ledToggle(void)
{
    cplBit(PORTD, PD7);
}

// =============================================================================
// Interrupt service routines
// =============================================================================

void adcConversionCompleteCallback(void)
{
    setBit(PORTD, PD4);
    timer1.clearCompareBInterruptRequest();
    systemFlags.rawTemperature = ADC;
    systemFlags.acquisitionCompleted = true;
    clrBit(PORTD, PD4);
}

void timer0CompareACallback(void)
{
    setBit(PORTD, PD5);
    mmc_disk_timerproc();
    clrBit(PORTD, PD5);
}

void timer1CompareACallback(void)
{
    cplBit(PORTD, PD3);
}

// =============================================================================
// END OF FILE
// =============================================================================
