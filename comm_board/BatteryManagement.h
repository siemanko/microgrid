/* 
 * File:   BatteryManagement.h
 * Author: Dan
 *
 * Created on December 20, 2014, 2:16 PM
 */

#ifndef BATTERYMANAGEMENT_H
#define	BATTERYMANAGEMENT_H

void initBattery(int debugMode);
void updateBatteryEnergy();
float getBatteryEnergy();
void setBatteryEnergy(float energyIn);
float getRemainingDailyEnergy();
float getTotalBatteryEnergy();
int getBatteryUpdateFrequency();

#endif	/* BATTERYMANAGEMENT_H */