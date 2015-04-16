/* 
 * File:   PowerStateManagement.h
 * Author: Dan
 *
 * Created on December 26, 2014, 11:26 AM
 */

#ifndef POWERSTATEMANAGEMENT_H
#define	POWERSTATEMANAGEMENT_H

//To keep track of what's producing what
float getExpectedRemainingDailySolarEnergy();
float getCurrentForecastIndex();

//To keep track of who's consuming what
void updateNodeEnergyConsumption(int UID, float energyConsumed);

#endif	/* POWERSTATEMANAGEMENT_H */

