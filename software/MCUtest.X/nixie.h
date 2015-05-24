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

extern volatile unsigned char ENCODER_STATE;

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



#define I2C_DATA_OUTPUT    TRICSbits.TRISC4;
#define I2C_DATA_CLEAR     TRISCbits.TRISC4 = 0; // pull SDA low
#define I2C_DATA_SET       TRISCbits.TRISC4 = 1; // let SDA high
#define I2C_DATA_PIN       RC4         // SDA pin

#define I2C_CLOCK_OUTPUT   TRISCbits.TRISC3;
#define I2C_CLOCK_CLEAR    TRISCbits.TRISC3 = 0; // pull SCL low
#define I2C_CLOCK_SET      TRISCbits.TRISC3 = 1; // let SCL high
#define I2C_CLOCK_PIN      RC3         // SCL pin

#define I2C_STATE_IDLE      0
#define I2C_STATE_ACTIVE    1

extern volatile unsigned char I2C_STATE;

// Init I2C
void i2c_init();

// I2C delay (minimum 5us)
// Timer0 is used in order to handle interrupts
inline void i2c_delay();
inline void i2c_begindelay();
inline void i2c_enddelay();

void i2c_start();
void i2c_stop();
void i2c_write_bit(char b);
char i2c_read_bit();
char i2c_write_byte(char byte); // Returns 0 if acknowledged by the slave
char i2c_read_byte(char nack);

#ifdef	__cplusplus
}
#endif

#endif	/* NIXIE_H */

