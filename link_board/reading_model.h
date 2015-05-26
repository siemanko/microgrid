
#ifndef READINGMODEL_H
#define	READINGMODEL_H

#define ADC_FORGET 0.1

void update_readings();

void on_battery_output_current_reading(double current_out_battery);
void on_network_voltage_reading(double output_voltage);
void on_battery_voltage_reading(double input_voltage);
void on_battery_input_current_reading(double current_into_battery);

double get_battery_output_current(void);
double get_network_voltage(void);
double get_battery_voltage(void);
double get_battery_input_current(void);

#endif	/* READINGMODEL_H */

