/* 
 * File:   LinkBoardInterface.h
 * Author: Dan
 *
 * Created on December 28, 2014, 5:02 PM
 */

#ifndef LINKBOARDINTERFACE_H
#define	LINKBOARDINTERFACE_H

void initLinkBoardInterface();

int getLinkBoardBatteryInputCurrent(float* result);
int getLinkBoardBatteryOutputCurrent(float* result);
int getLinkBoardNetworkVoltage(float* result);
int getLinkBoardBatteryVoltage(float* result);

#endif	/* LINKBOARDINTERFACE_H */

