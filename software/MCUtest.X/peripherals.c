/*
 * File:   peripherals.c
 * Author: Sebastian Kurella
 *
 * Created on 30.06.2015, 14:19
 */

#include <xc.h>

void peripherals_init() {
    RB1 = 0;    // buzzer off
    TRISB1 = 0; // set RB0 as output (buzzer)
    ANSELC = 0;
    TRISC0 = 0; // set RC0 as output (blue LED)
    TRISC1 = 0; // set RC1 as output (red LED)
    RC0 = 0;
    RC1 = 0;
    TRISC2 = 1; // set RC2 as input (SW1)
    ANSB3 = 0;
    TRISB3 = 1; // set RB3 as input (encoder PB)

    TRISC5 = 1; // RTC MFP - input
}

inline void buzzer_on() {
    RB1 = 1;
}

inline void buzzer_off() {
    RB1 = 0;
}

inline void blue_led_on() {
    RC0 = 1;
}

inline void blue_led_off() {
    RC0 = 0;
}

inline void red_led_on() {
    RC1 = 1;
}

inline void red_led_off() {
    RC1 = 0;
}

inline char encoder_button_state() {
    return !RB3;
}

inline char button_state() {
    return !RC2;
}