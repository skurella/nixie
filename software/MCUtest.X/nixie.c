/*
 * File:   nixie.c
 * Author: Sebastian Kurella
 *
 * Created on 22.05.2015, 19:54
 */

#include <xc.h>
#include "nixie.h"

volatile char current_tube = 0;

void nixie_init() {
    // anodes and digits as digital outputs
    ANSELA = 0;
    TRISA = 0;
    PORTA = inv_digit;

    // Initialize Timer2
    // Prescaler 1:4, postscaler 1:10
    // Refresh frequency 400Hz when counting to 250
    // (16 Mhz / 4) / (250 * 10 * 4) = 400 Hz
    T2CON = T2CKPS0;                // prescaler
    T2CON |= T2OUTPS3 | T2OUTPS0;   // postscaler
    TMR2IF = 0;
    PEIE = 1;
    TMR2IE = 1;                     // enable interrupts
    PR2 = 250;                      // Timer2 Period Register
    TMR2ON = 1;                     // start Timer2
}