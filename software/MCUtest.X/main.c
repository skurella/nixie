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

char hour = 0, min = 0; // raw BCD data read directly from RTC
char sethour, setmin;   // values of mins and hours used for setting time
char showhour, showmin; // to aid blinking during setting the time

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
            if (ui_timer == 6000) { // count max to one minute
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

void switch_state(char state) {
    // Switch state
    clock_state = state;

    // Reset the UI timer
    ui_timer = 0;

    // Some states need initialization

    // TODO: red LED when setting time, blue when setting alarm

    switch (state) {
        case CLOCK_STATE_IDLE:
            red_led_off();
            blue_led_off();
            break;
        case CLOCK_STATE_SETHOUR:
            red_led_on();
            blue_led_off();
            break;
        case CLOCK_STATE_ALMHOUR:
            red_led_off();
            blue_led_on();
            break;
    }
    switch (state) {
        case CLOCK_STATE_SETHOUR:
        case CLOCK_STATE_ALMHOUR:
            // Save hour to sethour and minute to setmin
            sethour = 10 * ((hour & 0x70) >> 4);
            sethour += hour & 0x0F;
            setmin = 10 * ((min & 0x70) >> 4);
            setmin += min & 0x0F;
            break;
    }
}

void display_set_time(char hours, char mins) {
    if (hours) {
        nixie_display(0, sethour / 10);
        nixie_display(1, sethour % 10);
    } else {
        nixie_display(0, 10);
        nixie_display(1, 10);
    }

    if (mins) {
        nixie_display(2, setmin / 10);
        nixie_display(3, setmin % 10);
    } else {
        nixie_display(2, 10);
        nixie_display(3, 10);
    }

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

    // Main loop
    while (1) {

        // TODO: return to idle if user is inactive for 1 minute

        switch (clock_state) {
            case CLOCK_STATE_IDLE:
                // TODO: check HT voltage for changes
                // If encoder switch gets pressed
                if (encoder_button_state() && !encoder_pressed) {
                    encoder_pressed = 1;
                    ui_timer = 0;
                    __delay_ms(5);
                }
                // If encoder switch gets depressed ;_;
                if (!encoder_button_state() && encoder_pressed) {
                    encoder_pressed = 0;
                    __delay_ms(5);
                    if (ui_timer > 100) {
                        switch_state(CLOCK_STATE_SETHOUR);
                        break;
                    } else {
                        switch_state(CLOCK_STATE_ALMHOUR);
                        break;
                    }
                }
                hour = rtc_get_hour();
                nixie_display(0, (hour & 0x70) >> 4);
                nixie_display(1, hour & 0x0F);
                min = rtc_get_min();
                nixie_display(2, (min & 0x70) >> 4);
                nixie_display(3, min & 0x0F);
                break;

            case CLOCK_STATE_SETHOUR:
                if (encoder_button_state() && !encoder_pressed) {
                    encoder_pressed = 1;
                    __delay_ms(5);
                }
                if (!encoder_button_state() && encoder_pressed) {
                    encoder_pressed = 0;
                    __delay_ms(5);
                    switch_state(CLOCK_STATE_SETMIN);
                }
                char state = encoder_updateState();
                if (state == ENCODER_TURN_CLOCKWISE)
                {
                    sethour = (sethour + 59) % 60;
                    display_set_time(showhour, 1);
                }
                if (state == ENCODER_TURN_COUNTERCLOCKWISE)
                {
                    sethour = (sethour + 1) % 60;
                    display_set_time(showhour, 1);
                }
                // TODO: call display_set_time every time
                char newshowhour = ui_timer % 100 < 70;
                if (newshowhour != showhour) {
                    showhour = newshowhour;
                    display_set_time(showhour, 1);
                }
                break;
                
            case CLOCK_STATE_SETMIN:
                if (encoder_button_state() && !encoder_pressed) {
                    encoder_pressed = 1;
                    __delay_ms(5);
                }
                if (!encoder_button_state() && encoder_pressed) {
                    encoder_pressed = 0;
                    __delay_ms(5);
                    hour =
                        (((sethour / 10) & 0xf) << 4) |
                        ((sethour % 10) & 0xf);
                    min =
                        (((setmin / 10) & 0xf) << 4) |
                        ((setmin % 10) & 0xf);
                    rtc_set_hour(hour);
                    rtc_set_min(min);
                    switch_state(CLOCK_STATE_IDLE);
                }
                char state = encoder_updateState();
                if (state == ENCODER_TURN_CLOCKWISE)
                {
                    setmin = (setmin + 59) % 60;
                    display_set_time(1, showmin);
                }
                if (state == ENCODER_TURN_COUNTERCLOCKWISE)
                {
                    setmin = (setmin + 1) % 60;
                    display_set_time(1, showmin);
                }
                // TODO: call display_set_time every time
                char newshowmin = ui_timer % 100 < 70;
                if (newshowmin != showmin) {
                    showmin = newshowmin;
                    display_set_time(1, showmin);
                }
                break;
                
            case CLOCK_STATE_ALARM:
                // TODO
                break;
            case CLOCK_STATE_ALMHOUR:
                // TODO
                break;
            case CLOCK_STATE_ALMMIN:
                // TODO
                break;
            case CLOCK_STATE_CAL:
                // TODO
                break;
            case CLOCK_STATE_VOLTAGE:
                // TODO
                break;
        }
        
    }

    //bulka
    red_led_on();
    __delay_ms(10000);

    return (EXIT_SUCCESS);
}

