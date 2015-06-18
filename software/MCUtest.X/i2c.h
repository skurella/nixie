/* 
 * File:   i2c.h
 * Author: Sebastian Kurella
 *
 * Created on 17.06.2015, 11:48
 */

#ifndef I2C_H
#define	I2C_H

#ifdef	__cplusplus
extern "C" {
#endif


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

char rtc_get_sec();
void rtc_set_sec(char sec);
char rtc_get_min();
void rtc_set_min(char hour);
char rtc_get_hour();
void rtc_set_hour(char hour);

void rtc_alm0_set_sec(char sec);
void rtc_alm0_set_mask(char mask);
char rtc_alm0_get_flag();
void rtc_alm0_clear_flag();
void rtc_alm0_enable();


#ifdef	__cplusplus
}
#endif

#endif	/* I2C_H */

