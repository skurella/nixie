/* 
 * File:   main.c
 * Author: Sebastian Kurella
 *
 * Created on 22.05.2015, 16:42
 */

// PIC16F1518 Configuration Bit Settings

// 'C' source line config statements

#include <xc.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select (MCLR/VPP pin function is MCLR)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is enabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config VCAPEN = OFF     // Voltage Regulator Capacitor Enable bit (VCAP pin function disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LPBOR = OFF      // Low-Power Brown Out Reset (Low-Power BOR is disabled)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (High-voltage on MCLR/VPP must be used for programming)


#define _XTAL_FREQ  16000000

#include <stdio.h>
#include <stdlib.h>

#include "nixie.h"
#include "i2c.h"
#include "encoder.h"


int main(int argc, char** argv) {

    OSCCONbits.IRCF = 0b1111;   // internal oscillator - 16 Mhz

    ANSELC = 0;
    TRISC0 = 0; // set RC0 as output (blue LED)
    TRISC1 = 0; // set RC1 as output (red LED)

    RC0 = 0;
    RC1 = 0;

    TRISC2 = 1; // set RC2 as input (SW1)

    ANSELA = 0;
    LATA = 0;
    TRISA = 0; // anodes and digits as digital outputs
    RA0 = 0;   // anode1
    RA1 = 0;   // anode2
    RA6 = 0;   // anode4
    RA7 = 0;   // anode3

    // Invalid digit (0xF)
    RA4 = 1;   // digit4
    RA3 = 1;   // digit3
    RA2 = 1;   // digit2
    RA5 = 1;   // digit1


    encoder_init();

    i2c_init();
    rtc_start();
    rtc_vbat_enable();
    
    while (1) {
        PORTA = 0b00000001 | 0b00100000;    // '1' on tube 1
        __delay_ms(1);
        PORTA = 0b00000010 | 0b00000100;    // '2' on tube 2
        __delay_ms(1);
        PORTA = 0b10000000 | 0b00100100;    // '3' on tube 3
        __delay_ms(1);
        PORTA = 0b01000000 | 0b00001000;    // '4' on tube 4
        __delay_ms(1);
    }

    return (EXIT_SUCCESS);
}

