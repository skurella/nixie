/* 
 * File:   main.c
 * Author: Sebastian Kurella
 *
 * Created on 22.05.2015, 16:42
 */

// PIC16F723A Configuration Bit Settings

// 'C' source line config statements

#include <xc.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // RE3/MCLR Pin Function Select bit (RE3/MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Selection bits (BOR disabled)
#pragma config BORV = 25        // Brown-out Reset Voltage selection bit (Brown-out Reset Voltage (VBOR) set to 2.5 V nominal)
#pragma config PLLEN = ON       // INTOSC PLL Enable bit (INTOSC Frequency is 16MHz (32x))

// CONFIG2
#pragma config VCAPEN = DIS     // Voltage Regulator Capacitor Enable bits (All VCAP pin functions are disabled)



#include <stdio.h>
#include <stdlib.h>

#include "nixie.h"


int main(int argc, char** argv) {

    TRISC0 = 0; // set RC0 as output (blue LED)
    TRISC1 = 0; // set RC1 as output (red LED)

    encoder_init();

    while (1) {
        switch (encoder_updateState()) {
        case ENCODER_TURN_CLOCKWISE:
            RC0 = 1;
            RC1 = 0;
            break;
        case ENCODER_TURN_COUNTERCLOCKWISE:
            RC0 = 0;
            RC1 = 1;
            break;
        }
    }
    
    return (EXIT_SUCCESS);
}

