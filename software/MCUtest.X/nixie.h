/* 
 * File:   nixie.h
 * Author: Sebastian Kurella
 *
 * Created on 22.05.2015, 19:54
 */

#ifndef NIXIE_H
#define	NIXIE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include <xc.h>

#define ENCODER_STATE_DENT      0
#define ENCODER_STATE_BEGINCW   1
#define ENCODER_STATE_MIDCW     2
#define ENCODER_STATE_ENDCW     3
#define ENCODER_STATE_BEGINCCW  4
#define ENCODER_STATE_MIDCCW    5
#define ENCODER_STATE_ENDCCW    6

#define ENCODER_TURN_NOTURN             0
#define ENCODER_TURN_CLOCKWISE          1
#define ENCODER_TURN_COUNTERCLOCKWISE   2

extern volatile char ENCODER_STATE;

// Init encoder inputs
void encoder_init();

// Returns encoder state machine state
inline unsigned char encoder_getState();

// Sets encoder state machine state
inline void encoder_setState(unsigned char state);

// Updates encoder state machine
// Return values:
// 0 - no change
// 1 - clockwise turn
// 2 - counterclockwise turn
unsigned char encoder_updateState();


typedef struct {
    char secone;
    char secten;
    char minone;
    char minten;
    char hrone;
    char hrten;
} time_t;

#define RTC_WRITE_ADDR      0b11011110
#define RTC_READ_ADDR       0b11011111
#define EEPROM_WRITE_ADDR   0b10101110
#define EEPROM_READ_ADDR    0b10101111


extern char I2C_DATA[8];

// Init I2C
void i2c_init();

// Delay until an I2C interrupt flag is set
void i2c_awaitIR();

// I2C bus control
void i2c_start();
void i2c_stop();
void i2c_restart();

// Main I2C functions
char i2c_send_byte(char byte);
char i2c_rec_byte();
void i2c_ack();
void i2c_nack();

// RTC specific functions
void rtc_write(char addr, char *bytes, int num);
void rtc_read(char addr, int num);
void rtc_start();
void rtc_vbat_enable();

#ifdef	__cplusplus
}
#endif

#endif	/* NIXIE_H */

