/*
 * File:   nixie.c
 * Author: Sebastian Kurella
 *
 * Created on 22.05.2015, 19:54
 */

#include <xc.h>
#include "nixie.h"

volatile unsigned char ENCODER_STATE;

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