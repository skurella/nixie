/* 
 * File:   peripherals.h
 * Author: Sebastian Kurella
 *
 * Created on 30.06.2015, 14:18
 */

#ifndef PERIPHERALS_H
#define	PERIPHERALS_H

#ifdef	__cplusplus
extern "C" {
#endif

// Init peripherals
void peripherals_init();

inline void buzzer_on();
inline void buzzer_off();

inline void blue_led_on();
inline void blue_led_off();

inline void red_led_on();
inline void red_led_off();

inline char encoder_switch_state();

inline char switch_state();


#ifdef	__cplusplus
}
#endif

#endif	/* PERIPHERALS_H */

