/*
 * File:   nixie.c
 * Author: Sebastian Kurella
 *
 * Created on 22.05.2015, 19:54
 */

#include <xc.h>
#include "nixie.h"

volatile unsigned char ENCODER_STATE;
volatile unsigned char I2C_STATE;

// Init encoder inputs
void encoder_init() {
    // Configure encoder input
    // RB3 - encoder PB
    // RB4 - encoder B
    // RB5 - encoder A
    OPTION_REGbits.nRBPU = 0;   // enable port B weak pull-up
    ANSELB &= 0b11000111;       // make RB3..5 digital
    WPUB3 = 1;                  // pull RB3 to VCC
    WPUB4 = 0;                  // disable RB4..5 pull-up (external is used)
    WPUB5 = 0;
    // CW turn starts with change on RB5
    // CCW turn starts with change on RB4

    encoder_setState(ENCODER_STATE_DENT);
}

// Returns encoder state machine state
inline unsigned char encoder_getState() {
    return ENCODER_STATE;
}

// Sets encoder state machine state
inline void encoder_setState(unsigned char state) {
    ENCODER_STATE = state;
}

// Updates encoder state machine
unsigned char encoder_updateState() {
    switch (ENCODER_STATE) {

    case ENCODER_STATE_DENT:
        if (RB5 && !RB4) {
            ENCODER_STATE = ENCODER_STATE_BEGINCCW;
        }
        if (RB4 && !RB5) {
            ENCODER_STATE = ENCODER_STATE_BEGINCW;
        }
        break;

    case ENCODER_STATE_BEGINCW:
        if ((RB5 && RB4) || (RB5 && !RB4)) {
            ENCODER_STATE = ENCODER_STATE_DENT;
        }
        if (!RB5 && !RB4) {
            ENCODER_STATE = ENCODER_STATE_MIDCW;
        }
        break;

    case ENCODER_STATE_MIDCW:
        if (RB5 && RB4) {
            ENCODER_STATE = ENCODER_STATE_DENT;
        }
        if (!RB5 && RB4) {
            ENCODER_STATE = ENCODER_STATE_BEGINCW;
        }
        if (RB5 && !RB4) {
            ENCODER_STATE = ENCODER_STATE_ENDCW;
        }
        break;

    case ENCODER_STATE_ENDCW:
        if (!RB5 && RB4) {
            ENCODER_STATE = ENCODER_STATE_DENT;
        }
        if (!RB5 && !RB4) {
            ENCODER_STATE = ENCODER_STATE_MIDCW;
        }
        if (RB5 && RB4) {
            ENCODER_STATE = ENCODER_STATE_DENT;
            return ENCODER_TURN_CLOCKWISE;
        }
        break;

    case ENCODER_STATE_BEGINCCW:
        if ((RB5 && RB4) || (!RB5 && RB4)) {
            ENCODER_STATE = ENCODER_STATE_DENT;
        }
        if (!RB5 && !RB4) {
            ENCODER_STATE = ENCODER_STATE_MIDCCW;
        }
        break;

    case ENCODER_STATE_MIDCCW:
        if (RB5 && RB4) {
            ENCODER_STATE = ENCODER_STATE_DENT;
        }
        if (RB5 && !RB4) {
            ENCODER_STATE = ENCODER_STATE_BEGINCCW;
        }
        if (!RB5 && RB4) {
            ENCODER_STATE = ENCODER_STATE_ENDCCW;
        }
        break;

    case ENCODER_STATE_ENDCCW:
        if (RB5 && !RB4) {
            ENCODER_STATE = ENCODER_STATE_DENT;
        }
        if (!RB5 && !RB4) {
            ENCODER_STATE = ENCODER_STATE_MIDCCW;
        }
        if (RB5 && RB4) {
            ENCODER_STATE = ENCODER_STATE_DENT;
            return ENCODER_TURN_COUNTERCLOCKWISE;
        }
        break;

    }

    return ENCODER_TURN_NOTURN;
}

// Init I2C
void i2c_init() {
    // Disable SSP module
    SSPCONbits.SSPEN = 0;
    
    // Clear output latches
    //I2C_DATA_PIN = 0; // for some reason these do not work as expected
    //I2C_CLOCK_PIN = 0;
    RC3 = 0;
    RC4 = 0;

    // Set both lines (idle)
    I2C_DATA_SET;
    I2C_CLOCK_SET;

    I2C_STATE = I2C_STATE_IDLE;

    // Configure Timer0
    OPTION_REGbits.T0CS = 0; // Timer0 clock source: instruction cycle clock
    OPTION_REGbits.PSA = 1; // assign prescaler to watchdog
}

// I2C delay (minimum 5us = 10 instruction cycles)
// Timer0 is used to handle interrupts
inline void i2c_delay() {
    i2c_begindelay();
    i2c_enddelay();
}

inline void i2c_begindelay() {
    TMR0 = 0;
    INTCONbits.TMR0IF = 0;
}

inline void i2c_enddelay() {
    while (TMR0 < 10 && !INTCONbits.TMR0IF);
}

void i2c_start() {
    // If I2C is already started, do a restart
    if (I2C_STATE != I2C_STATE_IDLE) {
        I2C_DATA_SET;
        i2c_delay();
        I2C_CLOCK_SET;
        while (I2C_CLOCK_PIN == 0); // clock stretching
        i2c_delay();
    }

    I2C_DATA_CLEAR;
    i2c_delay();
    I2C_CLOCK_CLEAR;

    I2C_STATE = I2C_STATE_ACTIVE;
}

void i2c_stop() {
    I2C_DATA_CLEAR;
    i2c_delay();
    I2C_CLOCK_SET;
    while (I2C_CLOCK_PIN == 0); // clock stretching
    i2c_delay();
    I2C_DATA_SET;
    i2c_delay();

    I2C_STATE = I2C_STATE_IDLE;
}

void i2c_write_bit(char b) {
    if (b) {
        I2C_DATA_SET;
    } else {
        I2C_DATA_CLEAR;
    }
    i2c_delay();
    I2C_CLOCK_SET;
    while (I2C_CLOCK_PIN == 0); // clock stretching
    // SCL is high, data is valid
    i2c_delay();
    I2C_CLOCK_CLEAR;
}

char i2c_read_bit() {
    char b;
    I2C_DATA_SET;
    i2c_delay();
    I2C_CLOCK_SET;
    while (I2C_CLOCK_PIN == 0); // clock stretching
    // SCL is high, data is valid
    b = I2C_DATA_PIN;
    i2c_delay();
    I2C_CLOCK_CLEAR;
    return b;
}

// Returns 0 if acknowledged by the slave
char i2c_write_byte(char byte) {
    for (char i = 0; i < 8; ++i) {
        i2c_write_bit((byte & 0x80) != 0);
        byte <<= 1;
    }
    return i2c_read_bit();
}

char i2c_read_byte(char nack) {
    char byte = 0;
    for (char i = 0; i < 8; ++i) {
        byte = (byte << 1) | i2c_read_bit();
    }
    i2c_write_bit(nack);
    return byte;
}

