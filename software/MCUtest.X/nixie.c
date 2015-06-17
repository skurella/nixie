/*
 * File:   nixie.c
 * Author: Sebastian Kurella
 *
 * Created on 22.05.2015, 19:54
 */

#include <xc.h>
#include "nixie.h"

volatile char ENCODER_STATE;

// Init encoder inputs
void encoder_init() {
    // Configure encoder input
    // RB3 - encoder PB
    // RB4 - encoder B
    // RB5 - encoder A
    OPTION_REGbits.nWPUEN = 0;   // enable port B weak pull-up
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


char I2C_DATA[8];

// Init I2C
void i2c_init() {
    
    // I2C Master Mode
    SSPCON1 = 0b00001000;

    /// Set Baud Rate Generator to 100kHz from 16MHz
    // 100 kHz = 16 MHz / (39 + 1)
    SSPADD = 39;

    // Enable SSP module
    SSPCON1bits.SSPEN = 1;

    // Clear the flags
    SSPIF = 0;
    WCOL = 0;
    SSPOV = 0;

    // Set both lines as inputs
    TRISC3 = 1;
    TRISC4 = 1;
    RC3 = 0;
    RC4 = 0;

    // Disable slew rate limiting
    SSPSTAT = 0b10000000;
}

// Delay until an I2C interrupt flag is set
void i2c_awaitIR(){
    // Wait for action to finish
    while (!PIR1bits.SSPIF);
    // Clear interrupt in software
    PIR1bits.SSPIF = 0;
}

// I2C bus control
void i2c_start() {
    SSPCON2bits.SEN = 1;
    i2c_awaitIR();
}

void i2c_stop() {
    SSPCON2bits.PEN = 1;
    i2c_awaitIR();
}

void i2c_restart() {
    SSPCON2bits.RSEN = 1;
    i2c_awaitIR();
}

// Main I2C functions
char i2c_send_byte(char byte) {
    SSPBUF = byte;
    i2c_awaitIR();

    return !SSPCON2bits.ACKSTAT;
}

char i2c_rec_byte() {
    SSPCON2bits.RCEN = 1;
    i2c_awaitIR();

    return SSPBUF;
}

void i2c_ack() {
    SSPCON2bits.ACKDT = 0;
    SSPCON2bits.ACKEN = 1;
    i2c_awaitIR();
}

void i2c_nack() {
    SSPCON2bits.ACKDT = 1;
    SSPCON2bits.ACKEN = 1;
    i2c_awaitIR();
}

// RTC specific functions
void rtc_write(char addr, char *bytes, int num) {
    i2c_start();
    // Send the slave address
    i2c_send_byte(RTC_WRITE_ADDR);
    // Send the SRAM address
    i2c_send_byte(addr);
    // Send the SRAM data
    for (int i = 0; i < num; ++i) {
        i2c_send_byte(bytes[i]);
    }
    i2c_stop();
}

void rtc_read(char addr, int num) {
    i2c_start();
    // Send the slave address
    i2c_send_byte(RTC_WRITE_ADDR);
    // Send the SRAM address
    i2c_send_byte(addr);
    i2c_restart();
    i2c_send_byte(RTC_READ_ADDR);
    // Receive the SRAM data
    for (int i = 0; i < num; ++i) {
        I2C_DATA[i] = i2c_rec_byte();
        // Acknowledge every byte but the last
        if (i < num - 1) {
            i2c_ack();
        } else {
            i2c_nack();
        }
    }
    i2c_stop();
}

void rtc_start() {
    rtc_read(0x00, 1);
    I2C_DATA[0] |= 0b10000000;
    rtc_write(0x00, I2C_DATA, 1);
}

void rtc_vbat_enable() {
    rtc_read(0x03, 1);
    I2C_DATA[0] |= 0b00001000;
    rtc_write(0x03, I2C_DATA, 1);
}