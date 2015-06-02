#ifndef CONSTANTS_H
#define CONSTANTS_H

// next time comm boards boots there's going to be a factory reset,
// and the next time and one time after that - 
// reprogramming with 0 will be required after programming
// with 1
#define FORCE_FACTORY_RESET 0
// some places in code have asserts - if assert fails
// the comm board stops executing and informs about the assert
// failure
// should probably be disabled in production.
#define ASSERTS_ENABLED  1

// this is to avoid hysteresis.
// minimum time to wait between swithing from red to yellow
// etc. does not apply to off state. If we think we 
// need to turn things off we turn them of after 
// one minute from last state change.
#define MIN_DELAY_BETWEEN_STATE_UPDATES_S 60 * 60

// uncomment for better performance debug data.
//#define CRON_STATS

// how often to log data
#define LOG_DATA_EVERY_S 10

// multiply coefficient for price
// at different states
#define PRICE_COEFF_GREEN  1
#define PRICE_COEFF_YELLOW 2
#define PRICE_COEFF_RED    10

#define DEFAULT_BATTERY_CAPACITY_Q 25200.0

// uncertainty of a current sensor is 2%
#define CURRENT_SENSOR_UNCERTAINTY 0.02
// uncertainty of a voltage sensor is 5%
#define VOLTAGE_SENSOR_UNCERTAINTY 0.05

#define INITIAL_BALANCE 10000

// thresholds
#define DEFAULT_OFF_THRESHOLD 0.5
#define DEFAULT_RED_THRESHOLD 0.55
#define DEFAULT_YELLOW_THRESHOLD 0.65

// used to determine if EEPROM bootup failed.
// should no be changed without factory reset.
#define STORAGE_INTEGRITY_CONSTANT 7LL

// default values for balance update.
#define DEFAULT_BALANCE_UPDATE_HOURS   2
#define DEFAULT_BALANCE_UPDATE_MINUTES 0
#define DEFAULT_BALANCE_UPDATE_AMMOUN  400000


#endif