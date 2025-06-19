#define F_CPU 16000000UL

#include "funsape/funsapeLibGlobalDefines.hpp"
#include "funsape/peripheral/funsapeLibGpioBus.hpp"
#include "funsape/peripheral/funsapeLibGpioPin.hpp"
#include "funsape/peripheral/funsapeLibTimer0.hpp"
#include "funsape/peripheral/funsapeLibPcint2.hpp"
#include "funsape/device/funsapeLibHd44780.hpp"

GpioPin led;
GpioPin buzzer;
Hd44780 display;

uint8_t pindAntigo = 0;
uint8_t dificuldade = 4;
uint8_t indiceInput = 0;
uint8_t tentativas = 0;
uint8_t pass[4];
uint8_t senhaUsuario[4];
uint8_t x, y;

bool_t gameOn = false;
bool_t botaoD_pressionado = false;
bool_t atualizaTelaSelecao = true;


void telaDeSelecao()
{
    display.clearScreen();
    printf("Dificuldade: %u", dificuldade);
    display.cursorGoTo(0, 13);
}

////////////////////////////////////////////////////////////////////////////////
// Realce de código – geração de senha secreta /////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
bool_t numeroJaExiste(uint8_t senha[], uint8_t tamanho, uint8_t numero)
{
    for(uint8_t i = 0; i < tamanho; ++i) {
        if(senha[i] == numero) {
            return true;
        }
    }
    return false;
}

void gerarSenha(uint8_t senha[4], uint8_t dificuldade)
{
    for(uint8_t i = 0; i < 4; ++i) {
        uint8_t candidato;
        do {
            candidato = (rand() % dificuldade) + 1;
        } while(numeroJaExiste(senha, i, candidato));

        senha[i] = candidato;
    }
}
////////////////////////////////////////////////////////////////////////////////
// Fim do Realce ///////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void mostraTentativas()
{
    display.cursorGoTo(1, 0);
    printf("%u ->%u.%u", tentativas, x, y);
}

void modoEntradaSenha()
{
    indiceInput = 0;

    for(uint8_t i = 0; i < 4; i++) {
        senhaUsuario[i] = 1;
    }

    display.clearScreen();
    display.cursorGoTo(0, 0);
    printf("Senha: %u%u%u%u", senhaUsuario[0], senhaUsuario[1], senhaUsuario[2], senhaUsuario[3]);

    mostraTentativas();

    display.cursorGoTo(0, 7);
}

void atualizaDisplayEntrada()
{
    display.cursorGoTo(0, 7);
    printf("%u%u%u%u", senhaUsuario[0], senhaUsuario[1], senhaUsuario[2], senhaUsuario[3]);
    display.cursorGoTo(0, 7 + indiceInput);
}

bool_t senhaUsuarioValida()
{
    for(uint8_t i = 0; i < 4; i++) {
        for(uint8_t j = i + 1; j < 4; j++) {
            if(senhaUsuario[i] == senhaUsuario[j]) {
                return false;
            }
        }
    }
    return true;
}

void comparaSenhas(uint8_t &x, uint8_t &y)
{
    x = 0;
    y = 0;
    for(uint8_t i = 0; i < 4; i++) {
        if(senhaUsuario[i] == pass[i]) {
            y++;
        } else {
            for(uint8_t j = 0; j < 4; j++) {
                if(i != j && senhaUsuario[i] == pass[j]) {
                    x++;
                    break;
                }
            }
        }
    }
}

int main()
{
    GpioPin gpioDisplayEn;
    GpioPin gpioDisplayRs;
    GpioPin gpioDisplayRw;
    GpioBus gpioDisplayData;

    led.init(&DDRB, GpioPin::PinIndex::P5);
    led.setMode(GpioPin::Mode::OUTPUT_PUSH_PULL);

    timer0.init(Timer0::Mode::NORMAL, Timer0::ClockSource::PRESCALER_1024);

    buzzer.init(&DDRC, GpioPin::PinIndex::P3);
    buzzer.setMode(GpioPin::Mode::OUTPUT_PUSH_PULL);
    buzzer.clr();

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

    pcint2.init(Pcint2::Pin::PIN_PCINT16 | Pcint2::Pin::PIN_PCINT17 | Pcint2::Pin::PIN_PCINT18);
    pcint2.clearInterruptRequest();
    pcint2.activateInterrupt();

    printf("Jogo da Senha!\n");
    printf("Henrique Onuki");
    delayMs(2000);
    pindAntigo = PIND;
    sei();

    display.clearScreen();

    display.setDisplayState(Hd44780::DisplayState::BLINK_ON);

    telaDeSelecao();

    while(true) {
        if(!gameOn && !botaoD_pressionado && atualizaTelaSelecao) {
            telaDeSelecao();
            atualizaTelaSelecao = false;
        }
        delayMs(100);
    }

    return 0;
}

void pcint2InterruptCallback(void)
{
    uint8_t pindAtual = PIND;
    uint8_t changedPins = pindAtual ^ pindAntigo;

    // Botao S
    if((changedPins & (1 << PD0)) && !(pindAtual & (1 << PD0))) {
        delayMs(10);
        if(!gameOn) {
            gameOn = true;
            srand(TCNT0);
            gerarSenha(pass, dificuldade);
            display.clearScreen();
            tentativas = 0;
            x = 0;
            y = 0;
            modoEntradaSenha();
        } else {
            if(indiceInput < 3) {
                indiceInput++;
                display.cursorGoTo(0, 7 + indiceInput);
            } else {
                if(!senhaUsuarioValida()) {
                    display.cursorGoTo(0, 0);
                    printf("Senha: XXXX");
                    buzzer.set();
                    delayMs(300);
                    buzzer.clr();
                    delayMs(1000);
                    modoEntradaSenha();
                    return;
                }

                tentativas++;

                x = 0, y = 0;
                comparaSenhas(x, y);

                display.cursorGoTo(0, 0);
                printf("Senha: %u%u%u%u", senhaUsuario[0], senhaUsuario[1], senhaUsuario[2], senhaUsuario[3]);
                display.cursorGoTo(1, 0);
                printf("%u ->%u.%u", tentativas, x, y);

                if(y == 4) {
                    for(uint8_t i = 0; i < 3; i++) {
                        buzzer.set();
                        delayMs(300);
                        buzzer.clr();
                        delayMs(100);
                    }
                    display.clearScreen();
                    printf("Parabens!");
                    delayMs(2000);
                    gameOn = false;
                    tentativas = 0;
                    telaDeSelecao();
                } else {
                    delayMs(1000);
                    modoEntradaSenha();
                }
                return;
            }
        }
    }
    // Botao I
    if((changedPins & (1 << PD1)) && !(pindAtual & (1 << PD1))) {
        delayMs(10);
        if(!gameOn) {
            dificuldade++;
            if(dificuldade > 9) {
                dificuldade = 4;
            }
            atualizaTelaSelecao = true;
        } else if(indiceInput < 4) {
            senhaUsuario[indiceInput]++;
            if(senhaUsuario[indiceInput] > dificuldade) {
                senhaUsuario[indiceInput] = 1;
            }
            atualizaDisplayEntrada();
        }

        led.toggle();
    }
    // Botao D
    if(changedPins & (1 << PD2)) {
        delayMs(10);
        if(!(pindAtual & (1 << PD2))) {
            // Botao D apertado
            botaoD_pressionado = true;
            display.clearScreen();
            display.cursorGoTo(0, 0);
            printf("Senha secreta:");
            display.cursorGoTo(1, 0);
            printf("%u%u%u%u", pass[0], pass[1], pass[2], pass[3]);
        } else {
            // Botao D solto
            botaoD_pressionado = false;
            if(gameOn) {
                modoEntradaSenha();
            } else {
                telaDeSelecao();
            }
        }
    }

    pindAntigo = pindAtual;
}
