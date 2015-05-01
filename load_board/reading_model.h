/* 
 * File:   Model.h
 * Author: sidor
 *
 * Created on January 23, 2015, 2:40 PM
 */

#ifndef MODEL_H
#define	MODEL_H

#define ADC_FORGET 0.001L

void on_output_current_reading(double current);
void on_network_voltage_reading(double network_voltage);
void on_output_voltage_reading(double output_voltage);
void on_phone_voltage_reading(double phone_voltage);

double get_output_current();
double get_network_voltage();
double get_output_voltage();
double get_phone_voltage();
#endif	/* MODEL_H */

