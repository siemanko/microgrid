#include <p33EP512GM710.h>
#include "LoadBoardInterface.h"
#include "SPI_LOAD_BOARD.h"
#include "NodalCommunicationInterface.h"
#include "LoadBoardConstants.h"
#include "NetworkCommConstants.h"
#include "GridStateManagement.h"
#include "ComputerCommunicationDriver.h"
#include "debugging.h"


void initLoadBoardInterface(){
    initSPI_LoadBoard();
}


int getLoadBoardOutputCurrent(float* result){
    return getFloatForCommand(GET_CURRENT_FROM_LOAD_BOARD_MSG, result);
}

int getLoadBoardNetworkVoltage(float* result){
    return getFloatForCommand(GET_LINE_VOLTAGE_FROM_LOAD_BOARD_MSG, result);
}

int getLoadBoardOutputVoltage(float* result){
    return getFloatForCommand(GET_PORT1_VOLTAGE_FROM_LOAD_BOARD_MSG, result);
}

int getLoadBoardPhoneVoltage(float* result){
    return getFloatForCommand(GET_PORT2_VOLTAGE_FROM_LOAD_BOARD_MSG, result);
}

int turnOffAllLoadBoardPorts(){
    int success = sendCommandToLoadBoard(TURN_PORT1_OFF_MSG) &&
                  sendCommandToLoadBoard(TURN_PORT2_OFF_MSG) &&
                  sendCommandToLoadBoard(TURN_LIGHT_OFF_MSG);
    debug(DEBUG_GRID_STATE, "SENT OFF SIGNAL TO APPLIANCES: %d", success);
    return success;
}

int turnOnAllLoadBoardPorts(){
    int success = sendCommandToLoadBoard(TURN_PORT1_ON_MSG) &&
                  sendCommandToLoadBoard(TURN_PORT2_ON_MSG) &&
                  sendCommandToLoadBoard(TURN_LIGHT_ON_MSG);
    debug(DEBUG_GRID_STATE, "SENT ON SIGNAL TO APPIANCES: %d", success);
    return success;
}