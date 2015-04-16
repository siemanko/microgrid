#include "SolarForecastModule.h"
#include "SPI_EEPROM.h"
#include "LibraryFunctions.h"
#include "debugging.h"

#define BYTES_PER_FLOAT                     4

#define SOLAR_FORECAST_START_ADDRESS        100

#define NUMBER_SOLAR_FORECASTS_PER_HOUR      2

#define TOTAL_SOLAR_FORECASTS                48

static int address;


void solar_forecast_upload_handle(char* msg) {
    int len = msg[1] - 4;
    int offset = BytesToInt(&(msg[2]));
    char* data = &(msg[4]);
    int start = SOLAR_FORECAST_START_ADDRESS + offset;
    int i;
    for(i=0; i<len; ++i) {
        // TODO: for some reasone delay_us does not work here...
        // need to figure out why...
        int j;
        for(j=0; j<10000; ++j);
        writeEEPROMByte(start+i, data[i]);
    }
    debug(DEBUG_GRID_STATE, "FORECAST @ %d (len:%d) WRITTEN (%d, %d)", 
            start, len, data[0], data[1]);
}


double getSingleSolarForecastValue(int index){
    double returnVal=-1;
    int addressRead=SOLAR_FORECAST_START_ADDRESS + index*BYTES_PER_FLOAT;   //Assume 0 index

    returnVal = readEEPROMFloat(addressRead);

    return returnVal;
}

//For now, these are treated as constants but should change to being read
//from EEPROM
//Returns sunrise in minutes from midnight
short getSunrise(){
    unsigned short sunrise = 388; //6:28 AM
    return sunrise;
}

//retruns sunset in minutes from midnight
short getSunset(){
    unsigned short sunset = 1520;   //5:20 PM 
    return sunset;
}

//Temporarily, for simplicity, I am assuming that solar power is constant
//over the entire day (between sunrise and sunset.  This will be replaced by a
//forecast or other better estimate (probably a Bayesian predictor)
float getSolarPower(){
    float constantEnergyPerHour = 100; //assume we produce at 100W
    return constantEnergyPerHour;
}

float getSolarDiscretization(){
    return (float)TOTAL_SOLAR_FORECASTS/24;
}