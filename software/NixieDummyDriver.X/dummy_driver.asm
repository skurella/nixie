;/////////////////////////////////////////////////////;
;/ Nixie tube clock driver dummy - simulates nixies  /;
;/ To be used with University of Manchester IO Board /;
;/ Author: Sebastian Kurella                         /;
;/////////////////////////////////////////////////////;



        processor   18F8722
        #include    <P18F8722.INC>
        config      OSC=HS, WDT=OFF, LVP=OFF
        radix       decimal

; reset vector - this is the entry point of the program
        ORG         0x0000
        goto        Main

; high priority interrupt vector
        ORG         0x0008
        BTG         LATH, 1         ; toggle Q1 - switch U2 on or off
        BCF         INTCON, TMR0IF  ; clear Timer0 overflow flag
        MOVLW       0x67            ; Set initial timer value to have 1 sec left
        MOVWF       TMR0H           ; TODO: restore WREG!
        MOVLW       0x69
        MOVWF       TMR0L
        retfie

Main    BCF         TRISH, 0        ; set RH0 as output - Q1 transistor
        BCF         TRISH, 1        ; set RH1 as output - Q2 transistor
        BCF         LATH, 0         ; pull RH0 value low - enable U1
        BSF         LATH, 1         ; pull RH1 value high - disable U2

        MOVLW       B'01100100'     ; set RF output values - display '3'
        MOVWF       LATF
        MOVLW       0               ; set RF as output
        MOVWF       TRISF

; set up Timer0
        MOVLW       B'00000101'     ; Timer Off, 16bit, internal clock, prescaler 1:64
        MOVWF       T0CON
        MOVLW       0x67            ; Set initial timer value to have 1 sec left
        MOVWF       TMR0H
        MOVLW       0x69
        MOVWF       TMR0L
        BSF         INTCON, TMR0IE  ; Enable overflow interrupt
        BSF         INTCON2, TMR0IP ; High interrupt priority

        BSF         RCON, IPEN      ; Enable interrupt priority
        BSF         INTCON, GIE     ; Enable high-priority interrupts
        BSF         T0CON, TMR0ON   ; Start Timer0



L1      BRA         L1              ; loop forever

        end
