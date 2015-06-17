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

const char digit[10] = {
    0b00000000, // 0
    0b00100000, // 1
    0b00000100, // 2
    0b00100100, // 3
    0b00001000, // 4
    0b00101000, // 5
    0b00001100, // 6
    0b00101100, // 7
    0b00010000, // 8
    0b00110000  // 9
};

const char inv_digit = 0b00111100;

const char tube[4] = {
    0b00000001, // tube 1
    0b00000010, // tube 2
    0b10000000, // tube 3
    0b01000000  // tube 4
};

extern volatile char current_tube;

void nixie_init();

#ifdef	__cplusplus
}
#endif

#endif	/* NIXIE_H */

