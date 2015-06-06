#ifndef CONSTANTS_H
#define CONSTANTS_H

#define ASSERTS_ENABLED  1
//#define CRON_STATS

#define LOG_DATA_EVERY_S 1

#define PRICE_COEFF_GREEN  1
#define PRICE_COEFF_YELLOW 2
#define PRICE_COEFF_RED    10
#define PRICE_COEFF_OFF    0

#define DEFAULT_BATTERY_CAPACITY_Q 25200.0

// uncertainty of a current sensor is 2%
#define CURRENT_SENSOR_UNCERTAINTY 0.02
// uncertainty of a voltage sensor is 5%
#define VOLTAGE_SENSOR_UNCERTAINTY 0.05

#define INITIAL_BALANCE 10000
#define INITIAL_PRICE   20

// thresholds
#define DEFAULT_OFF_THRESHOLD 0.5
#define DEFAULT_RED_THRESHOLD 0.55
#define DEFAULT_YELLOW_THRESHOLD 0.65


#define STORAGE_INTEGRITY_CONSTANT 7LL

#define FORCE_FACTORY_RESET 0

#endif