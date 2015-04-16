#include "UserManagement.h"
#include "UserInteractionConstants.h"
#include "GridStateManagement.h"

void initUserManagement(){
    GRID_NON_CRITICAL_OUTPUT=0;
    GRID_CRITICAL_OUTPUT=0;
}

//How much time is predicted to be left on the activity
//In hours
float getExpectedRemainingDemandTime(int UID){
    float hoursLeft = 1;
    return  hoursLeft;
}

//Returns the expected remaining energy demand for the current day
//For now, we assume we don't know about anyone plugging anything else in
float getExpectedRemainingDemand(){
    int numConnectedNodes = getNumberOfConnectedNodes();
    float expectedRemainingDemand = 0;
    int i;
    Node ** nodes = getConnectedNodes();

    for(i=0;i<numConnectedNodes;i++){
        expectedRemainingDemand+=(*nodes)[i].powerConsumption*1;    
    }

    return expectedRemainingDemand;
}

//Just a constant and stub for now
float getExpectedLightingEnergy(){

    //Some simple constant assumptions
    int users = 4;
    int hoursWithLights = 3;
    int powerPerLight = 5;

    unsigned int energyDemandedForLights = users*hoursWithLights*powerPerLight; //In W*hr
    return energyDemandedForLights;
}