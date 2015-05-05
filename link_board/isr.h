/* 
 * File:   isr.h
 * Author: sidor
 *
 * Created on January 26, 2015, 10:27 PM
 */

#ifndef ISR_H
#define	ISR_H

#include <stdint.h>

extern int readings_updated;
extern uint16_t battery_output_current_adc;
extern uint16_t battery_input_current_adc;
extern uint16_t battery_voltage_adc;
extern uint16_t network_voltage_adc;

#endif	/* ISR_H */

