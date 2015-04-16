#include <p33EP512GM710.h>
#include <math.h>

#include "DataLogger.h"
#include "SPI_EEPROM.h"
#include "settings.h"
#include "LinkBoardInterface.h"
#include "LoadBoardInterface.h"
#include "ComputerCommunicationDriver.h"
#include "DateAndTime.h"
#include "debugging.h"

#define BYTES_IN_FLOAT                      4
#define RECORDINGS_PER_DATA_POINT           6

#define LOGGING_START_TIMESTAMP   300
#define LAST_LOG_TIMESTAMP        304
#define NEXT_LOG_INDEX            308
#define LOGGING_BASE              310
#define LOGGING_CAPACITY          32000

#define DATA_LOGGER_DISABLE       0

#define NAN 0.0/0.0

unsigned long last_log_ts = 0;
int update_frequency_s;

void initDataLogger(int frequency_s) {
    update_frequency_s = frequency_s;
    last_log_ts = readEEPROMLong(LAST_LOG_TIMESTAMP);

    register_computer_message_handler(RESET_DATALOGGER, reset_data_logging_handler);
}

void setLastLogTs(unsigned long ts) {
    last_log_ts = ts;
    writeEEPROMLong(LAST_LOG_TIMESTAMP, ts);
}

void setNextLogIndex(unsigned int index) {
    writeEEPROMInt(NEXT_LOG_INDEX, index);
}

void dataLoggerStep(void) {
    if (DATA_LOGGER_DISABLE) return;
    unsigned long time = getTimestamp();

    if (time >= last_log_ts + update_frequency_s) {

        unsigned short next_logging_index = readEEPROMInt(NEXT_LOG_INDEX);
        if (next_logging_index > LOGGING_CAPACITY) {
            return;
        }
        debug(DEBUG_DATALOGGER, "Logged at index %d", next_logging_index);


        if (getSetting(SETTINGS_NODE_TYPE)=='A'){
            float battery_input_current;
            float battery_output_current;
            float network_voltage;
            float battery_voltage;

            int success =
                    getLinkBoardBatteryInputCurrent(&battery_input_current) &&
                    getLinkBoardBatteryOutputCurrent(&battery_output_current) &&
                    getLinkBoardNetworkVoltage(&network_voltage) &&
                    getLinkBoardBatteryVoltage(&battery_voltage);

            if (!success) {
                battery_input_current = NAN;
                battery_output_current = NAN;
                network_voltage = NAN;
                battery_voltage = NAN;
            }

            writeEEPROMFloat(next_logging_index, &battery_input_current);
            next_logging_index+=4;
            writeEEPROMFloat(next_logging_index, &battery_output_current);
            next_logging_index+=4;
            writeEEPROMFloat(next_logging_index, &network_voltage);
            next_logging_index+=4;
            writeEEPROMFloat(next_logging_index, &battery_voltage);
            next_logging_index+=4;
        } else if (getSetting(SETTINGS_NODE_TYPE) == 'B') {
            float output_current;
            float network_voltage;
            float output_voltage;
            float phone_voltage;

            int success =
                    getLoadBoardOutputCurrent(&output_current) &&
                    getLoadBoardNetworkVoltage(&network_voltage) &&
                    getLoadBoardOutputVoltage(&output_voltage) &&
                    getLoadBoardPhoneVoltage(&phone_voltage);

            if (!success) {
                output_current = NAN;
                network_voltage = NAN;
                output_voltage = NAN;
                phone_voltage = NAN;
            }
            writeEEPROMFloat(next_logging_index, &output_current);
            next_logging_index+=4;
            writeEEPROMFloat(next_logging_index, &network_voltage);
            next_logging_index+=4;
            writeEEPROMFloat(next_logging_index, &output_voltage);
            next_logging_index+=4;
            writeEEPROMFloat(next_logging_index, &phone_voltage);
            next_logging_index+=4;
        }

        setLastLogTs(time);
        setNextLogIndex(next_logging_index);
    }
}

void get_data_logs_handler() {

}

void reset_data_logging_handler() {
    if (DATA_LOGGER_DISABLE) return;
    setLastLogTs(0);
    writeEEPROMLong(LOGGING_START_TIMESTAMP, getTimestamp());
    setNextLogIndex(LOGGING_BASE);
    debug(DEBUG_DATALOGGER, "DATALOGGER RESET");
}