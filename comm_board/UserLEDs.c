#include "UserLEDs.h"
#include "UserInteractionConstants.h"

void signalGridNotCritical(){
    GRID_NON_CRITICAL_WRITE = 1;
    GRID_CRITICAL_WRITE = 0;
}

void signalGridCritical(){
    GRID_NON_CRITICAL_WRITE = 0;
    GRID_CRITICAL_WRITE = 1;
}