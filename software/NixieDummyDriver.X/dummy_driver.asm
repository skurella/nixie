;/////////////////////////////////////////////////////;
;/ Nixie tube clock driver dummy - simulates nixies  /;
;/ To be used with University of Manchester IO Board /;
;/ Author: Sebastian Kurella                         /;
;/                                                   /;
;/ PB2 displays hours, PB1 displays minutes          /;
;/ Low nibble of SW1 is simulated 74141 input        /;
;/ High nibble of SW1 is simulated 'tube select'     /;
;/////////////////////////////////////////////////////;



        processor   18F8722
        #include    <P18F8722.INC>
        config      OSC=HS, WDT=OFF, LVP=OFF
        radix       decimal

        CBLOCK	0x80

        HoursT      ; tens digit of current hour
        HoursU      ; units digit of current hour
        MinutesT    ; tens digit of current minute
        MinutesU    ; units digit of current minute

        DispView    ; 0 - hours, 1 - minutes

        LDispValue  ; digit to be displayed on right display
        RDispValue  ; digit to be displayed on right display

        WREG_TEMP	; variables used for context saving
        STATUS_TEMP
        BSR_TEMP

        ENDC

; reset vector - this is the entry point of the program
        ORG     0x0000
        goto    Init

; high priority interrupt vector
        ORG     0x0008
        ; high-p int code goes here
        retfie  FAST

; low priority interrupt vector
        ORG     0x0018
        goto    LowInt



; BCDTable is a lookup table which converts BCD to RF config
; in order to display this digit on 7-segment display

BCDTable    sublw   9           ; check if number is within BCD range
            btfss   STATUS, 0   ; if WREG was > 9, it's invalid
            retlw   B'11111011'
            sublw   9           ; return WREG to original state

            movf    PCL, F      ; update PC registers
            rlncf   WREG, W     ; multiply by 2 to get the correct offset
            addwf   PCL         ; Add offset to force jump into table
            retlw   B'10000100' ; '0'
            retlw   B'11110101' ; '1'
            retlw   B'01001100' ; '2'
            retlw   B'01100100' ; '3'
            retlw   B'00110101' ; '4'
            retlw   B'00100110' ; '5'
            retlw   B'00000110' ; '6'
            retlw   B'11110100' ; '7'
            retlw   B'00000100' ; '8'
            retlw   B'00100100' ; '9'





; initialize the microcontroller

Init    bcf     TRISH, 0        ; set RH0 as output - Q1 transistor
        bcf     TRISH, 1        ; set RH1 as output - Q2 transistor
        bcf     LATH, 0         ; pull RH0 value low - enable U1
        bsf     LATH, 1         ; pull RH1 value high - disable U2

        movlw   B'11111111'     ; set RF output values - all segments off
        movwf   LATF
        movlw   0               ; set RF as output
        movwf   TRISF

        bsf     TRISJ, 5        ; set RJ5 as input - PB1
        bsf     TRISB, 0        ; set RB0 as input - PB2

        movlw   B'00111100'     ; set RC2,3,4,5 as inputs - SW1H
        iorwf   TRISC
        movlw   B'11110000'     ; set RH4,5,6,7 as inputs - SW1L
        iorwf   TRISH
        movlw   B'00000011'     ; set RH4-7 as digital I/O
        iorwf   ADCON1

; set up initial time value
        movlw   1
        movwf   HoursT
        movlw   2
        movwf   HoursU
        movlw   3
        movwf   MinutesT
        movlw   4
        movwf   MinutesU

; initially display hours
        CALL    DispH

; set up Timer0
; display switch frequency: (10 Mhz / 4) / 64 / 2^8 = 152.6 Hz
        movlw   B'01000101'     ; Timer off, 8bit, internal clock, prescaler 1:64
        movwf   T0CON
        bsf     INTCON, TMR0IE  ; Enable overflow interrupt
        bcf     INTCON2, TMR0IP ; Low interrupt priority

        bsf     RCON, IPEN      ; Enable interrupt priority
        bsf     INTCON, GIE     ; Enable high-priority interrupts
        bsf     INTCON, PEIE    ; Enable low-priority interrupts
        bsf     T0CON, TMR0ON   ; Start Timer0


; main loop
; continuously monitor push-buttons and toggle switches

MLoop
        btfss   PORTB, 0        ; show hours if PB2 is pressed
        CALL    DispH
        btfss   PORTJ, 5        ; show minures if PB1 is pressed
        CALL    DispM

        movf    PORTH, W        ; read SW1H
        andlw   B'11110000'
        bz MLoop                ; return if none is selected

        rrncf   PORTC, W        ; read SW1L value
        rrncf   WREG
        andlw   B'00001111'

        btfsc   PORTH, 7        ; set tens of hours if SW1a is on
        movwf   HoursT
        btfsc   PORTH, 6        ; set units of hours if SW1b is on
        movwf   HoursU
        btfsc   PORTH, 5        ; set tens of minutes if SW1c is on
        movwf   MinutesT
        btfsc   PORTH, 4        ; set units of minutes if SW1d is on
        movwf   MinutesU

        CALL    DRefr

        bra     MLoop           ; execute main loop forever




; low priority interrupt saves and restores working registers
; because FAST stack is used by high priority interrupts

LowInt  movff   STATUS, STATUS_TEMP	;save STATUS register
        movff   WREG, WREG_TEMP		;save working register
        movff   BSR, BSR_TEMP		;save BSR register

        btfss   INTCON, TMR0IF      ; branch if Timer0 not OV
        bra     LIntRet

        bcf     INTCON, TMR0IF      ; clear Timer0 overflow flag

        btg     LATH, 0             ; toggle Q1 - switch U1 on or off
        btg     LATH, 1             ; toggle Q2 - switch U2 on or off
        btfss   LATH, 0             ; check which display is to be switched on
        bra     RDisp

        ; handle left display
        movff   LDispValue, LATF
        bra     LIntRet

        ; handle right display
RDisp   movff   RDispValue, LATF

LIntRet movff   BSR_TEMP, BSR		;restore BSR register
        movff	WREG_TEMP, WREG		;restore working register
        movff	STATUS_TEMP, STATUS	;restore STATUS register

        retfie


; this subroutine refreshes the displayed values
DRefr   btfss   DispView, 0
        CALL    DispH
        btfsc   DispView, 0
        CALL    DispM
        return

; this subroutine places Hours [BCD] in DispValue registers [7seg]
DispH   movf    HoursT, W       ; convert BCD to 7seg
        CALL    BCDTable
        movwf   LDispValue      ; place tens in left display
        movf    HoursU, W
        CALL    BCDTable
        movwf   RDispValue      ; place units in right display
        bcf     DispView, 0     ; set hours view
        return

; this subroutine places Minutes [BCD] in DispValue registers [7seg]
DispM   movf    MinutesT, W       ; convert BCD to 7seg
        CALL    BCDTable
        movwf   LDispValue      ; place tens in left display
        movf    MinutesU, W
        CALL    BCDTable
        movwf   RDispValue      ; place units in right display
        bsf     DispView, 0     ; set minutes view
        return


        END
