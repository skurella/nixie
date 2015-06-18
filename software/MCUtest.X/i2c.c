/*
 * File:   i2c.c
 * Author: Sebastian Kurella
 *
 * Created on 17.06.2015, 11:51
 */

#include <xc.h>
#include "i2c.h"

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

char rtc_get_sec() {
    rtc_read(0x00, 1);
    I2C_DATA[0] &= 0x7F;    // mask ST bit
    return I2C_DATA[0];
}

void rtc_set_sec(char sec) {
    I2C_DATA[0] = sec;
    rtc_write(0x00, I2C_DATA, 1);
}

char rtc_get_min() {
    rtc_read(0x01, 1);
    return I2C_DATA[0];
}
void rtc_set_min(char min) {
    I2C_DATA[0] = min;
    rtc_write(0x01, I2C_DATA, 1);
}

char rtc_get_hour() {
    rtc_read(0x02, 1);
    if (I2C_DATA[0] & 0x40) {   // if 12 hours are used
        return I2C_DATA[0] & 0x1F;  // mask AM/PM bit
    } else {
        return I2C_DATA[0] & 0x3F;  // include HRTEN1 bit
    }
}
void rtc_set_hour(char hour) {
    I2C_DATA[0] = hour;
    rtc_write(0x02, I2C_DATA, 1);
}

void rtc_alm0_set_sec(char sec) {
    I2C_DATA[0] = sec;
    rtc_write(0x0A, I2C_DATA, 1);
}

void rtc_alm0_set_mask(char mask) {
    rtc_read(0x0D, 1);
    I2C_DATA[0] &= 0x8F;    // mask the ALM0MSK bits
    I2C_DATA[0] |= (mask << 4);
    rtc_write(0x0D, I2C_DATA, 1);
}

char rtc_alm0_get_flag() {
    rtc_read(0x0D, 1);
    return (I2C_DATA[0] & 0x08) >> 3;
}
void rtc_alm0_clear_flag() {
    rtc_read(0x0D, 1);
    I2C_DATA[0] &= 0xF7;    // mask the ALM0IF bit
    rtc_write(0x0D, I2C_DATA, 1);
}

void rtc_alm0_enable() {
    rtc_read(0x07, 1);    // read CONTROL register
    I2C_DATA[0] |= 0x10;    // set the ALM0EN bit
    rtc_write(0x07, I2C_DATA, 1);
}