/* 
 * File:   UserManagement.h
 * Author: Dan
 *
 * Created on December 26, 2014, 12:35 PM
 */

#ifndef USERMANAGEMENT_H
#define	USERMANAGEMENT_H

void initUserManagement();

float getExpectedRemainingDemandTime(int UID);      //Just a stub for now
float currentPredictedEnergyDemand();

float getExpectedRemainingDemand();
float getExpectedLightingEnergy();


#endif	/* USERMANAGEMENT_H */

