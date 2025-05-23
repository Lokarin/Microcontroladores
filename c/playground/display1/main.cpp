#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibGpioBus.hpp"
#include "funsape/peripheral/funsapeLibGpioPin.hpp"
#include "funsape/device/funsapeLibHd44780.hpp"
#include "funsape/device/funsapeLibKeypad.hpp"

int main()
{
    // Variaveis
    Hd44780 lcd;
    GpioPin lcdGpioPinE;
    GpioPin lcdGpioPinRs;
    GpioBus lcdGpioBusData;

    // init dos pinos e bus
    lcdGpioPinE.init(&DDRC, GpioPin::PinIndex::P5);
    lcdGpioPinRs.init(&DDRC, GpioPin::PinIndex::P4);
    lcdGpioBusData.init(&DDRC, GpioBus::PinIndex::P0, 4);

    // LCD config
    lcd.setControlPort(&lcdGpioPinE, &lcdGpioPinRs);
    lcd.setDataPort(&lcdGpioBusData);

    lcd.init(Hd44780::Size::LCD_16X2, Hd44780::Font::FONT_5X8);

    lcd.stdio(); // isso seta lcd como a saida padrao MUITO IMPORTANTE
    lcd.clearScreen();
    lcd.cursorHome();
    printf("Oi");
    delayMs(1000);
    lcd.clearScreen();
    printf("Quem leu\nmamou\n");
    
    //lcd.displayShift(Direction::RIGHT);
    //if (!lcd.displayShift(Direction::RIGHT)) {
    //    Error lastError = lcd.getLastError();
    //    printf("ERR=%04X\n\n", (uint16_t)lastError);
    //}

    setBit(DDRB, PB5);

    while(1) {
        cplBit(PORTB, PB5);
        delayMs(500);
    }

    return 0;
}
