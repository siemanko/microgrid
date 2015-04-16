/* 
 * File:   DispatchingExecutive.h
 * Author: Dan
 *
 * Created on December 20, 2014, 2:04 PM
 */

#ifndef DISPATCHINGEXECUTIVE_H
#define	DISPATCHINGEXECUTIVE_H

//Some of these should be static!

void initDispatcher(long sysFrequency,int debugMode, int myUID);
void DetermineDispatch(int requestingUID, float requestedEnergy);
int isGridInSafeMode();


#endif	/* DISPATCHINGEXECUTIVE_H */

