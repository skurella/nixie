;/////////////////////////////////////////////////////;
;/ Nixie tube clock driver dummy - simulates nixies  /;
;/ To be used with University of Manchester IO Board /;
;/ Author: Sebastian Kurella                         /;
;/////////////////////////////////////////////////////;



        processor   18F8722
        #include    <P18F8722.INC>
        config      OSC=HS, WDT=OFF, LVP=OFF
        radix       decimal

        CBLOCK	0x080
        WREG_TEMP	;variable used for context saving
        STATUS_TEMP	;variable used for context saving
        BSR_TEMP	;variable used for context saving
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
        goto     LowInt

; initialize the microcontroller
Init    bcf     TRISH, 0        ; set RH0 as output - Q1 transistor
        bcf     TRISH, 1        ; set RH1 as output - Q2 transistor
        bcf     LATH, 0         ; pull RH0 value low - enable U1
        bsf     LATH, 1         ; pull RH1 value high - disable U2

        movlw   B'11111111'     ; set RF output values - all segments off
        movwf   LATF
        movlw   0               ; set RF as output
        movwf   TRISF

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


L1      bra     L1              ; loop forever


; low priority interrupt saves and restores working registers
; because FAST stack is used by high priority interrupts
LowInt  movff   STATUS, STATUS_TEMP	;save STATUS register
        movff   WREG, WREG_TEMP		;save working register
        movff   BSR, BSR_TEMP		;save BSR register

        btfss   INTCON, TMR0IF      ; branch if Timer0 not OV
        bra     LIntRet

        bcf     INTCON, TMR0IF  ; clear Timer0 overflow flag

        btg     LATH, 0         ; toggle Q1 - switch U1 on or off
        btg     LATH, 1         ; toggle Q2 - switch U2 on or off
        btfss   LATH, 0         ; check which display is to be switched on
        bra     RDisp

        ; handle left display
        movlw   8               ; set RF output values - display '8'
        CALL    BCDTable
        movwf   LATF
        bra     LIntRet

        ; handle right display
RDisp   movlw   2               ; set RF output values - display '2'
        CALL    BCDTable
        movwf   LATF

LIntRet movff   BSR_TEMP, BSR		;restore BSR register
        movff	WREG_TEMP, WREG		;restore working register
        movff	STATUS_TEMP, STATUS	;restore STATUS register

        retfie


; BCDTable is a lookup table which converts BCD to RF config
; in order to display this digit on 7-segment display

BCDTable    movf    PCL, F      ; update PC registers
            rlncf   WREG, W        ; multiply by 2 to get the correct offset
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


        END
