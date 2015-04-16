/* 
 * File:   LoadBoardCommInterface.h
 * Author: Dan
 *
 * Created on December 20, 2014, 4:54 PM
 */

#ifndef LOADBOARDINTERFACE_H
#define	LOADBOARDINTERFACE_H

void initLoadBoardInterface();

int getLoadBoardOutputCurrent(float* result);
int getLoadBoardNetworkVoltage(float* result);
int getLoadBoardOutputVoltage(float* result);
int getLoadBoardPhoneVoltage(float* result);
int turnOffAllLoadBoardPorts();
int turnOnAllLoadBoardPorts();


#endif	/* LOADBOARDCOMMINTERFACE_H */

