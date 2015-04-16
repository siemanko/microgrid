/* 
 * File:   SolarForecastModule.h
 * Author: Dan
 *
 * Created on December 26, 2014, 11:32 AM
 */

#ifndef SOLARFORECASTMODULE_H
#define	SOLARFORECASTMODULE_H

void solar_forecast_upload_handle(char* msg);
double getSingleSolarForecastValue(int index);
short getSunrise();
short getSunset();
float getSolarPower();
float getSolarDiscretization();


#endif	/* SOLARFORECASTMODULE_H */

