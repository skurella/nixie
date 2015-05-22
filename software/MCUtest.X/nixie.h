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

#ifdef	__cplusplus
}
#endif

#endif	/* NIXIE_H */

