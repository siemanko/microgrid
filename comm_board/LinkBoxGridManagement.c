#include "LinkBoxGridManagement.h"

#include <stdint.h>

#include "LinkBoardInterface.h"
#include "debugging.h"

static int should_results_be_used = 0;

static int num_bad_readings_in_row = 0;

static float battery_input_current;
static float battery_output_current;
static float network_voltage;
static float battery_voltage;

static uint32_t grid_management_last_update;



void initLinkBoxGridManagment() {
    grid_management_last_update = 0;
    should_results_be_used = 0;
    num_bad_readings_in_row = 0;
}

static int areReadingsNumericallySensible() {
    int battery_input_current_normal = -0.1 <= battery_input_current && battery_input_current <= 100.0;
    int battery_output_current_normal =  -0.1 <= battery_output_current && battery_output_current <= 100.0;
    int network_voltage_normal = 0.0 <= network_voltage && network_voltage <= 50.0;
    int battery_voltage_normal = 0.0 <= battery_voltage && battery_voltage <= 50.0;
    return battery_input_current_normal &&
           battery_output_current_normal &&
           network_voltage_normal &&
           battery_voltage_normal;
}

int LinkBoxShouldUsePowerReadings() {
    return should_results_be_used;
}

float LinkBoxGetBatteryNetChange() {
    return (battery_input_current-battery_output_current)*battery_voltage;
}

static void updateReadings() {
    should_results_be_used =
            getLinkBoardBatteryInputCurrent(&battery_input_current) &&
            getLinkBoardBatteryOutputCurrent(&battery_output_current) &&
            getLinkBoardNetworkVoltage(&network_voltage) &&
            getLinkBoardBatteryVoltage(&battery_voltage);
    should_results_be_used = should_results_be_used &&
            areReadingsNumericallySensible();
    if (!should_results_be_used) {
        debug(DEBUG_GRID_STATE,
                "Problem getting reading from link board");
        ++num_bad_readings_in_row;
    } else {
        num_bad_readings_in_row = 0;
    }
}

void LinkBoxGridManagmentStep() {
    if (grid_management_last_update + 1 <= getTimestamp()) {
        grid_management_last_update = getTimestamp();
        updateReadings();
        if (LinkBoxShouldUsePowerReadings()) {
            debug(DEBUG_GRID_STATE, "Power consumed last second %f",
                                    LinkBoxGetBatteryNetChange());
            debug(DEBUG_GRID_STATE, "Battery voltage: %f", battery_voltage);
        }
    }
}