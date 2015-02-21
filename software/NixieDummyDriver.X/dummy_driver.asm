        processor   18F8722
        #include    <P18F8722.INC>
        config      OSC=HS, WDT=OFF, LVP=OFF
        radix       decimal

; reset vector - this is the entry point of the program
        ORG         0x0000
        goto        Main

Main    BCF         TRISH, 0        ; set RH0 as output - Q1 transistor
        BCF         TRISH, 1        ; set RH1 as output - Q2 transistor
        BCF         LATH, 0         ; pull RH0 value low - enable U1
        BSF         LATH, 1         ; pull RH1 value high - disable U2

        MOVLW       B'01100100'     ; set RF output values - display '3'
        MOVWF       LATF
        MOVLW       0               ; set RF as output
        MOVWF       TRISF

L1      BRA         L1              ; loop forever

        end
