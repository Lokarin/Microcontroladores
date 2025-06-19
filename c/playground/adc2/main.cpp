#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibGpioBus.hpp"
#include "funsape/peripheral/funsapeLibGpioPin.hpp"
#include "funsape/peripheral/funsapeLibAdc.hpp"
#include "funsape/peripheral/funsapeLibTimer0.hpp"
#include "funsape/device/funsapeLibHd44780.hpp"

vuint16_t potRaw = 0;

typedef union {
    struct {
        bool_t isAdcReady       : 1;
        uint8_t unusedBites     : 7;
    };
    uint8_t allFlags;
} systemFlags_t;

volatile systemFlags_t systemFlags;


int main()
{
    Hd44780 display;

    GpioPin gpioDisplayEn;
    GpioPin gpioDisplayRs;
    GpioPin gpioDisplayRw;
    GpioBus gpioDisplayData;

    systemFlags.allFlags = 0;

    gpioDisplayEn.init(&DDRC, GpioPin::PinIndex::P0);
    gpioDisplayRs.init(&DDRC, GpioPin::PinIndex::P1);
    gpioDisplayRw.init(&DDRC, GpioPin::PinIndex::P2);
    gpioDisplayData.init(&DDRB, GpioBus::PinIndex::P0, 4);

    display.setControlPort(
            &gpioDisplayEn,
            &gpioDisplayRs,
            &gpioDisplayRw
    );
    display.setDataPort(&gpioDisplayData);
    display.init(Hd44780::Size::LCD_16X2);
    display.stdio();
    printf("--Teste do LCD--\n0123456789ABCDEF\n");
    delayMs(2000);
    display.clearScreen();

    // TIMER1 configuration
    timer0.setCompareBValue(156-1);
    timer0.setCompareAValue(156-1);
    timer0.init(Timer0::Mode::CTC_OCRA, Timer0::ClockSource::PRESCALER_1024);
    timer0.clearCompareAInterruptRequest();
    timer0.clearCompareBInterruptRequest();

    // ADC configuration
    adc.init(
            Adc::Mode::AUTO_TIMER0_COMPA,
            Adc::Reference::POWER_SUPPLY,
            Adc::Prescaler::PRESCALER_128
    );
    adc.setChannel(Adc::Channel::CHANNEL_3);
    adc.clearInterruptRequest();
    adc.activateInterrupt();
    adc.enable();

    sei();

    while(true) {
        if(systemFlags.isAdcReady) {
            uint32_t aux32 = (uint32_t)potRaw;
            aux32 *= 625;
            aux32 /= 128;
            uint16_t potValue = (uint16_t)aux32;
            display.cursorHome();
            if (potValue > 1000) {
                printf("ADC=%04u\nV=%u.%03u V\n", potRaw, potValue / 1000, potValue % 1000);
            } else {
                printf("ADC=%04u\nV=%u mV\n", potRaw, potValue);
            }
            //printf("fds=%u\n", potValue);
            systemFlags.isAdcReady = false;
        }
    }

    return 0;
}

void adcConversionCompleteCallback(void)
{
    timer0.clearCompareBInterruptRequest();
    potRaw = ADC;
    systemFlags.isAdcReady = true;

}
