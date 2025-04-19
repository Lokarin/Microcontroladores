; ------------------------------------------------------------------------------
; Project: Exemplo de animacao com leds
; File: main.asm
; Author: Henrique Amaral Onuki
; Created: 2025-04-19
; Modified: 2025-04-19
; Version: 1.a
; Purpose: Uma animação de leds usando o ATMega328P
; Notes: This code is prepared to deal with Arduino Nano old bootloaders.
; ------------------------------------------------------------------------------

; ------------------------------------------------------------------------------
; Include definition files
; ------------------------------------------------------------------------------
.include "include/m328Pdef.inc"

; ------------------------------------------------------------------------------
; Interrupt vectors
; ------------------------------------------------------------------------------
.org 0x0000
    JMP resetHandler
.org INT0addr
    JMP int0Handler
.org INT1addr
    JMP int1Handler
.org PCI0addr
    JMP pcint0Handler
.org PCI1addr
    JMP pcint1Handler
.org PCI2addr
    JMP pcint2Handler
.org WDTaddr
    JMP wdtHandler
.org OC2Aaddr
    JMP timer2CompAHandler
.org OC2Baddr
    JMP timer2CompBHandler
.org OVF2addr
    JMP timer2OvfHandler
.org ICP1addr
    JMP timer1CaptureHandler
.org OC1Aaddr
    JMP timer1CompAHandler
.org OC1Baddr
    JMP timer1CompBHandler
.org OVF1addr
    JMP timer1OvfHandler
.org OC0Aaddr
    JMP timer0CompAHandler
.org OC0Baddr
    JMP timer0CompBHandler
.org OVF0addr
    JMP timer0OvfHandler
.org SPIaddr
    JMP spiHandler
.org URXCaddr
    JMP usartRxHandler
.org UDREaddr
    JMP usartDataEmptyHandler
.org UTXCaddr
    JMP usartTxHandler
.org ADCCaddr
    JMP adcHandler
.org ERDYaddr
    JMP eepromHandler
.org ACIaddr
    JMP analogCompHandler
.org TWIaddr
    JMP twiHandler
.org SPMRaddr
    JMP flashHandler
.org INT_VECTORS_SIZE

; ------------------------------------------------------------------------------
; Include other assembly files
; ------------------------------------------------------------------------------
; NONE

; ------------------------------------------------------------------------------
; Register definitions
; ------------------------------------------------------------------------------
.def auxReg = R16

; ------------------------------------------------------------------------------
; Constant definitions
; ------------------------------------------------------------------------------
; NONE

; ------------------------------------------------------------------------------
; Constants stored in Flash memory
; Note: Variables must be multiples of 2, since memory is organized in 16 bits
; ------------------------------------------------------------------------------
; NONE

; ------------------------------------------------------------------------------
; Data memory variables
; ------------------------------------------------------------------------------
; NONE

; ------------------------------------------------------------------------------
; Main code
; ------------------------------------------------------------------------------
main:
    ;Startup area
    LDI     auxReg, 0xFF        ;Preparando para mudar o estado da porta D
    OUT     DDRD, auxReg        ;Porta D em output mode
    OUT     PORTD, R17          ;Porta D em output LOW mode 

mainLoop:
    
animationStart:
    LDI     auxReg, 0b10000000  ;Primeiro frame, LED0 ligado
    
animationLoop:
    OUT     PORTD, auxReg       ;'mostra' o primeiro frame
    RCALL   delayMs250          ;pula para a subrotina de delay
    LSR     auxReg              ;um frame para a direita
    BREQ    animationStart      ;IF auxReg==0, animacao acabou
    RJMP    animationLoop       ;ELSE, mais um loop

; ------------------------------------------------------------------------------
; Function/Subroutine definition
; ------------------------------------------------------------------------------
delayMs250:
    NOP                     ; Comment this line for CALL / Uncomment for RCALL
    LDI     R18, 21
    LDI     R19, 75
    LDI     R20, 188
    RJMP    PC + 1
delayMs250Loop:
    DEC     R20
    BRNE    delayMs250Loop
    DEC     R19
    BRNE    delayMs250Loop
    DEC     R18
    BRNE    delayMs250Loop
    RET

; ------------------------------------------------------------------------------
; Interrupt handlers
; ------------------------------------------------------------------------------
int0Handler:
    RETI ; Returns from interrupt
int1Handler:
    RETI ; Returns from interrupt
pcint0Handler:
    RETI ; Returns from interrupt
pcint1Handler:
    RETI ; Returns from interrupt
pcint2Handler:
    RETI ; Returns from interrupt
wdtHandler:
    RETI ; Returns from interrupt
timer2CompAHandler:
    RETI ; Returns from interrupt
timer2CompBHandler:
    RETI ; Returns from interrupt
timer2OvfHandler:
    RETI ; Returns from interrupt
timer1CaptureHandler:
    RETI ; Returns from interrupt
timer1CompAHandler:
    RETI ; Returns from interrupt
timer1CompBHandler:
    RETI ; Returns from interrupt
timer1OvfHandler:
    RETI ; Returns from interrupt
timer0CompAHandler:
    RETI ; Returns from interrupt
timer0CompBHandler:
    RETI ; Returns from interrupt
timer0OvfHandler:
    RETI ; Returns from interrupt
spiHandler:
    RETI ; Returns from interrupt
usartRxHandler:
    RETI ; Returns from interrupt
usartDataEmptyHandler:
    RETI ; Returns from interrupt
usartTxHandler:
    RETI ; Returns from interrupt
adcHandler:
    RETI ; Returns from interrupt
eepromHandler:
    RETI ; Returns from interrupt
analogCompHandler:
    RETI ; Returns from interrupt
twiHandler:
    RETI ; Returns from interrupt
flashHandler:
    RETI ; Returns from interrupt
resetHandler:
    LDI     auxReg, 0               ; Fixing some Arduino Nano bootloaders
    STS     UCSR0B, auxReg          ; that have forgotten to disable USART0
    LDI     auxReg, LOW(RAMEND)     ; Let’s put the stack pointer at the end
    OUT     SPL, auxReg             ; of the RAM, just to be safe...
    LDI     auxReg, HIGH(RAMEND)    ; ...
    OUT     SPH, auxReg             ; ...
    JMP     main                    ; Now, let’s start the code