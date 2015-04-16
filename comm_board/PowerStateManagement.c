#include "PowerStateManagement.h"
#include "DateAndTime.h"
#include "SolarForecastModule.h"

#define MINUTES_PER_DAY         1440
#define MINUTES_PER_HOUR        60
#define HOURS_PER_DAY           24

float getExpectedRemainingDailySolarEnergy(){

    float currentTimeInHours = (60*getHours() + getMinutes())/60;
    short currentForecastIndex = currentTimeInHours*getSolarDiscretization();
    //Get energy from solar scenarios
    float energyRemaining=0;
    float solarForecastHourlyLength = 1/(float)getSolarDiscretization();

    int i;
    for(i = currentForecastIndex+1;i<HOURS_PER_DAY*getSolarDiscretization();i++){
        energyRemaining+=getSingleSolarForecastValue(i);
    }
    energyRemaining*=solarForecastHourlyLength;
    
    return energyRemaining;
}

float getCurrentForecastIndex(){
   float currentTimeInHours = (float)getMinutes()/60+(float)getHours();
   int currentForecastIndex = currentTimeInHours*getSolarDiscretization();
   return currentForecastIndex;
}


void updateNodeEnergyConsumption(int UID, float energyConsumed){



}