/* 
 * File:   main.c
 * Author: Sebastian Kurella
 *
 * Created on 22.05.2015, 16:42
 */

#include "peripherals.h"


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


#define _XTAL_FREQ  1000000

#include <stdio.h>
#include <stdlib.h>

#include "nixie.h"
#include "i2c.h"
#include "encoder.h"

#define CLOCK_STATE_IDLE    0   // display the time
#define CLOCK_STATE_SETHOUR 1   // set hours of the time
#define CLOCK_STATE_SETMIN  2   // set minutes of the time
#define CLOCK_STATE_ALARM   3   // sound the alarm
#define CLOCK_STATE_ALMHOUR 4   // set hours of alarm
#define CLOCK_STATE_ALMMIN  5   // set minutes of alarm
#define CLOCK_STATE_CAL     6   // calibrate the clock
#define CLOCK_STATE_VOLTAGE 7   // display HT voltage

volatile char clock_state = CLOCK_STATE_IDLE;
volatile unsigned int ui_timer = 0;
char encoder_pressed = 0;
char button_pressed = 0;

void interrupt tc_int() {
    while (1) {
        if (TMR0IE && TMR0IF) {
            TMR0IF = 0;
            ++current_tube;
            current_tube %= 4;

            PORTA = display[current_tube];
            continue;
        }

        if (TMR2IE && TMR2IF) {
            TMR2IF = 0;
            ++ui_timer;
            if (ui_timer == 60000) { // count max to one minute
                ui_timer = 0;
            }

            continue;
        }
        
        return;
    }
}

void Timer2_init() {
    // Initialize Timer2
    // Prescaler 1:4, postscaler 1:2
    // Refresh frequency 400Hz when counting to 250
    // (1 Mhz / 1) / (250 * 10 * 4) = 100 Hz

    T2CONbits.T2CKPS = 0b00;    // prescaler
    T2CONbits.T2OUTPS = 10;      // postscaler

    TMR2IF = 0;
    TMR2IE = 1;
    PEIE = 1;
    PR2 = 250;
    TMR2ON = 1;
}

int main() {
    // Set internal oscillator to 1 Mhz
    OSCCONbits.IRCF = 0b1011;

    peripherals_init();

    nixie_init();

    encoder_init();

    i2c_init();
    rtc_start();
    rtc_vbat_enable();

    // Clear the display
    nixie_clear();

    // Start asynchronous timer
    Timer2_init();
    
    // Enable interrupts
    ei();

    char hour = 0, min = 0;

    // Main loop
    while (1) {

        switch (clock_state) {
            case CLOCK_STATE_IDLE:
                // TODO: check HT voltage for changes
                // TODO: check encoder button for short or long press

                hour = rtc_get_hour();
                nixie_display(0, (hour & 0x70) >> 4);
                nixie_display(1, hour & 0x0F);
                min = rtc_get_min();
                nixie_display(2, (min & 0x70) >> 4);
                nixie_display(3, min & 0x0F);
                break;

            case CLOCK_STATE_SETHOUR:
                break;
            case CLOCK_STATE_SETMIN:
                break;
            case CLOCK_STATE_ALARM:
                break;
            case CLOCK_STATE_ALMHOUR:
                break;
            case CLOCK_STATE_ALMMIN:
                break;
            case CLOCK_STATE_CAL:
                break;
            case CLOCK_STATE_VOLTAGE:
                break;
        }
        
    }

    return (EXIT_SUCCESS);
}

