/* 
 * File:   GridManagment.h
 * Author: sidor
 *
 * Created on January 23, 2015, 10:04 PM
 */

#ifndef GRIDMANAGMENT_H
#define	GRIDMANAGMENT_H

int LoadBoxIsPowerConsumed();

float LoadBoxGetPower();

void initLoadBoxGridManagement();

int LoadBoxShouldUsePowerReadings();

void LoadBoxGridManagementStep();

#endif	/* GRIDMANAGMENT_H */

