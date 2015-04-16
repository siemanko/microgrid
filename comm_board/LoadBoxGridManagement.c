#include "LoadBoxGridManagement.h"

#include "DateAndTime.h"
#include "LoadBoardInterface.h"
#include "Balance.h"
#include "debugging.h"


#define BAD_READINGS_BEFORE_SWITCHOFF 5

int should_results_be_used = 0;

int num_bad_readings_in_row = 0;

// current at the output of load converter
// the current reading does include all loads but does not include
// current consumed by boards.
float output_current;
// voltage at the input to power converter, should be more or less the same
// for all boards.
float network_voltage;
// this is voltage at the output of load converter. It is also the voltage
// used at port1.
float output_voltage;
// voltage at port2
float phone_voltage;

uint32_t grid_management_last_update;

int LoadBoxIsPowerConsumed() {
    return output_current > 0.02;
}

float LoadBoxGetPower() {
    return output_current * output_voltage;
}

void initLoadBoxGridManagement() {
    grid_management_last_update = 0;
    should_results_be_used = 0;
    num_bad_readings_in_row = 0;
}

int areReadingsNumericallySensible() {
    int output_current_normal = 0.0 <= output_current && output_current <= 1000.0;
    int network_voltage_normal =  0.1 <= network_voltage && network_voltage <= 60.0;
    int output_voltage_normal = 0.0 <= output_voltage && output_voltage <= 20.0;
    int phone_voltage_normal = 0.0 <= phone_voltage && phone_voltage <= 10.0;
    return output_current_normal &&
           network_voltage_normal &&
           output_voltage_normal &&
           phone_voltage_normal;
}

int LoadBoxShouldUsePowerReadings() {
    return should_results_be_used;
}

void updateReadings() {
    should_results_be_used =
            getLoadBoardOutputCurrent(&output_current) &&
            getLoadBoardNetworkVoltage(&network_voltage);
            getLoadBoardOutputVoltage(&output_voltage);
            getLoadBoardPhoneVoltage(&phone_voltage);
    should_results_be_used = should_results_be_used &&
            areReadingsNumericallySensible();
    if (!should_results_be_used) {
        debug(DEBUG_GRID_STATE,
                "Problem getting reading from load board");
        ++num_bad_readings_in_row;
    } else {
        num_bad_readings_in_row = 0;
    }
}

void LoadBoxGridManagementStep() {
    if (grid_management_last_update + 1 <= getTimestamp()) {
        grid_management_last_update = getTimestamp();

        updateReadings();
        
        if (LoadBoxShouldUsePowerReadings()) {
            if (getBalance() == 0 && LoadBoxIsPowerConsumed()) {
                turnOffAllLoadBoardPorts();
                // return;
            }
            if (getBalance() > 0 && !LoadBoxIsPowerConsumed()) {
                turnOnAllLoadBoardPorts();
            }
        } else {
            if (num_bad_readings_in_row > BAD_READINGS_BEFORE_SWITCHOFF)
            turnOffAllLoadBoardPorts();
        }
    } 

}

