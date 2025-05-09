; ------------------------------------------------------------------------------
; Project: pushLed em assembly
; File: main.asm
; Author: Henrique Amaral Onuki
; Created: 2025-05-07
; Modified: 2025-05-07
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
.def pusherReg = R17  
.def pushedReg = R18 
.def tempReg = R19     
.def delayReg1 = R20   
.def delayReg2 = R21   
.def delayReg3 = R22   

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
    LDI     auxReg, 0xFF        ; todos os pinos como saida
    OUT     DDRD, auxReg        ; configura PORTD como saida
    LDI     auxReg, 0x00        ; leds desligados
    OUT     PORTD, auxReg       ; atualiza PORTD

    ; inicio da animacao
    LDI     pushedReg, 0x02     ; inicializa o pushed no led 1 0b00000010

mainLoop:
    LDI     pusherReg, 0x01     ; inicializa o pusher no led 0 0b00000001

pusherLoop:
    ; mostra os leds pusher e pushed
    MOV     tempReg, pusherReg
    OR      tempReg, pushedReg  ; combina pusher e pushed. Resultado 0b00000011
    OUT     PORTD, tempReg      ; atualiza os leds
    
    RCALL   delayMs250          ; delay
    
    ; desloca o pusher para a esquerda
    LSL     pusherReg           ; (logical shift left)
    
    ; verifica se o pusher ta em cima do pushed
    CP      pusherReg, pushedReg
    BRNE    pusherLoop          ; se sao estiver (vulgo nao sao iguais), continua o loop do pusher
    
    ; mas se forem iguais:
    OUT     PORTD, pushedReg    ; Mostra apenas o led pushed, que é a mesma coisa que mostrar os dois nesse momento
    RCALL   delayMs250          ; delay
    
    ; atualiza o pushed para o proximo led para a esquerda
    LSL     pushedReg           ; move o pushed um bit para a esquerda
    
    ; verifica se o pushed saiu dos 8 leds, ficaria 0
    CPI     pushedReg, 0x00
    BRNE    mainLoop            ; se nao for 0, continua com o comecamos o ciclo dnv, sem alterar os leds atuais
    
    ; se o pushed saiu dos leds, ai sim alteramos o estado atual dos leds
    LDI     pushedReg, 0x02     ; reinicia o pushed no led 1 0b00000010
    RJMP    mainLoop            ; volta ao inicio da animacao

; ------------------------------------------------------------------------------
; Function/Subroutine definition
; ------------------------------------------------------------------------------
delayMs250:
    LDI     delayReg1, 21
    LDI     delayReg2, 75
    LDI     delayReg3, 188
    RJMP    PC + 1
delayMs250Loop:
    DEC     delayReg3
    BRNE    delayMs250Loop
    DEC     delayReg2
    BRNE    delayMs250Loop
    DEC     delayReg1
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
    LDI     auxReg, LOW(RAMEND)     ; Let's put the stack pointer at the end
    OUT     SPL, auxReg             ; of the RAM, just to be safe...
    LDI     auxReg, HIGH(RAMEND)    ; ...
    OUT     SPH, auxReg             ; ...
    JMP     main                    ; Now, let's start the code