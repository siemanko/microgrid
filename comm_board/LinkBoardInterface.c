#include "LinkBoardInterface.h"

#include "LinkBoardConstants.h"
#include "SPI_LOAD_BOARD.h"



void initLinkBoardInterface(int debugMode){
    initSPI_LoadBoard();
}

int getLinkBoardBatteryInputCurrent(float* result){
    return getFloatForCommand(LINK_BATTERY_INPUT_CURRENT, result);
}

int getLinkBoardBatteryOutputCurrent(float* result){
    return getFloatForCommand(LINK_BATTERY_OUTPUT_CURRENT, result);
}

int getLinkBoardNetworkVoltage(float* result){
    return getFloatForCommand(LINK_NETWORK_VOLTAGE, result);
}

int getLinkBoardBatteryVoltage(float* result){
    return getFloatForCommand(LINK_BATTERY_VOLTAGE, result);
}

