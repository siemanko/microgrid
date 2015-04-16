#include "BatteryManagement.h"
#include "LinkBoardInterface.h"

#define BATTERY_UPDATE_DELTA_T 1    //Battery Energy Updated Every 1 Second

#define TOTAL_BATTERY_ENERGY_WHR   1200

static float batteryEnergy;

static int debug_mode;

void initBattery(int debugMode){
    debug_mode=debugMode;
    batteryEnergy = TOTAL_BATTERY_ENERGY_WHR;
}

void updateBatteryEnergy(){
    float inCurrent;
    float outCurrent;    
    
    if(!debug_mode){
        inCurrent = 0.0; // getCurrentIntoBatteryFromLinkBoard();
        outCurrent = 0.0; //getCurrentOutOfBatteryFromLinkBoard();
    }else{
        inCurrent = 7.5;
        outCurrent=7.5;
    }

    batteryEnergy+=BATTERY_UPDATE_DELTA_T*(inCurrent-outCurrent);
}

float getBatteryEnergy(){
    return batteryEnergy;
}

void setBatteryEnergy(float energyIn){
    batteryEnergy = energyIn;
}

//JUST A STUB FOR NOW!  Need to figure out how to make a prediction
//This function provides us with how much more energy the solar panel will provide
//us for the remainder of the day.
float getRemainingDailyEnergy(){
    return 20;
}

//This is the battery's charged energy
//And thus is a constant
//For the pilot we are using 200A*hr batteries
float getTotalBatteryEnergy(){
    float totalBatteryEnergy = TOTAL_BATTERY_ENERGY_WHR;  //Whr, 200Ahr*12V = 2400 Whr
    return totalBatteryEnergy;
}

int getBatteryUpdateFrequency(){
    return BATTERY_UPDATE_DELTA_T;
}
