/*
 * File:   nixie.c
 * Author: Sebastian Kurella
 *
 * Created on 22.05.2015, 19:54
 */

#include <xc.h>
#include "nixie.h"

volatile char current_tube = 0;

volatile char display[4];

void nixie_init() {
    // anodes and digits as digital outputs
    ANSELA = 0;
    TRISA = 0;
    PORTA = inv_digit;

    
    OPTION_REGbits.PS = 1;      // Timer0 prescaler 1:2^(PS+1)
    OPTION_REGbits.T0CS = 0;    // Clock Source - instruction clock
    OPTION_REGbits.PSA = 0;     // assign prescaler to Timer0
    TMR0IF = 0;
    TMR0IE = 1;
    PEIE = 1;                   // enable interrupts
}

void nixie_display(char tube, char digit) {
    tube %= 4;
    if (digit > 9) {
        display[tube] = nixie_tube[tube] | inv_digit;
    } else {
        display[tube] = nixie_tube[tube] | nixie_digit[digit];
    }
}

void nixie_clear() {
    for (char i = 0; i < 4; ++i) {
        nixie_display(10, i);
    }
}