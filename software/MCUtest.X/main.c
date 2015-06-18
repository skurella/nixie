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


#define _XTAL_FREQ  500000

#include <stdio.h>
#include <stdlib.h>

#include "nixie.h"
#include "i2c.h"
#include "encoder.h"

volatile int number = 1234;

char display[4];

void interrupt tc_int() {
    if (TMR2IE && TMR2IF) {
        TMR2IF = 0;
        ++current_tube;
        current_tube %= 4;
        
        PORTA = display[current_tube];
        return;
    }
}

void update_display() {
    display[0] = tube[0] | digit[(number / 1000) % 10];
    display[1] = tube[1] | digit[(number / 100) % 10];
    display[2] = tube[2] | digit[(number / 10) % 10];
    display[3] = tube[3] | digit[number % 10];
}

int main(int argc, char** argv) {
    // Set internal oscillator to 1 Mhz
    //OSCCONbits.IRCF = 0b1011;

    // Initialize peripherals
    ANSELC = 0;
    TRISC0 = 0; // set RC0 as output (blue LED)
    TRISC1 = 0; // set RC1 as output (red LED)
    RC0 = 0;
    RC1 = 0;
    TRISC2 = 1; // set RC2 as input (SW1)

    nixie_init();

    encoder_init();

    i2c_init();
    rtc_start();
    rtc_vbat_enable();

    TRISC5 = 1; // RTC MFP - input
    
    // Enable interrupts
    ei();

    update_display();
    char hour = 0, min = 0;

    rtc_alm0_set_sec(0);
    rtc_alm0_set_mask(0b000);
    rtc_alm0_enable();

    while (1) {
        hour = rtc_get_hour();
        display[0] = tube[0] | digit[(hour & 0x70) >> 4];
        display[1] = tube[1] | digit[hour & 0x0F];
        min = rtc_get_min();
        display[2] = tube[2] | digit[(min & 0x70) >> 4];
        display[3] = tube[3] | digit[min & 0x0F];
    }

    return (EXIT_SUCCESS);
}

