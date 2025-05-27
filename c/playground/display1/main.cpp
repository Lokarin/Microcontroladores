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
    Keypad keybad;

    GpioBus keybadGpioRows;
    GpioBus keybadGpioColumns;

    // init dos pinos e bus
    lcdGpioPinE.init(&DDRC, GpioPin::PinIndex::P5);
    lcdGpioPinRs.init(&DDRC, GpioPin::PinIndex::P4);
    lcdGpioBusData.init(&DDRC, GpioBus::PinIndex::P0, 4);

    keybadGpioRows.init(&DDRB, GpioBus::PinIndex::P0, 4);


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


    // Keybad config
    keybad.setPorts(&keybadGpioRows, &keybadGpioColumns);
    keybad.setKeyValues(Keypad::Type::KEYPAD_4X4,
            0X07, 0x08, 0x09, 0x0A,
            0X04, 0x05, 0x06, 0x0B,
            0X01, 0x02, 0x03, 0x0C,
            0X0F, 0x00, 0x0E, 0x0D

    );
    keybad.init(10);


    //lcd.displayShift(Direction::RIGHT);
    //if (!lcd.displayShift(Direction::RIGHT)) {
    //    Error lastError = lcd.getLastError();
    //    printf("ERR=%04X\n\n", (uint16_t)lastError);
    //}

    setBit(DDRB, PB5);

    while(1) {
        uint8_t keyPressed = 0xFF;
        keybad.readKeyPressed(&keyPressed);

        switch(keyPressed) {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x08:
        case 0x09:
        case 0x0A:
        case 0x0B:
        case 0x0C:
        case 0x0D:
        case 0x0E:
        case 0x0F:
            printf("%x", keyPressed);
        default:
            break;
        }

        cplBit(PORTB, PB5);
        delayMs(500);
    }

    return 0;
}
