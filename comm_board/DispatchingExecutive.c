#include <p33EP512GM710.h>
#include "DispatchingExecutive.h"
#include "NodalCommunicationInterface.h"
#include "LoadBoardInterface.h"
#include "NodalCommunicationInterface.h"
#include "PowerStateManagement.h"
#include "UserManagement.h"
#include "DateAndTime.h"
#include "BatteryManagement.h"
#include "GridStateManagement.h"
#include "SolarForecastModule.h"
#include "NetworkCommConstants.h"
#include "ComputerCommunicationDriver.h"
#include "ComputerMessages.h"

#define MINIMUM_BATTERY_ENERGY                  300     //Whr
#define NON_CRITICAL_ENERGY_LEVEL               1000    //Whr

#define CONTINUED_DISPATCH_FLAG_TIMER           5

static unsigned int dispatchTimerFlag;
static unsigned long system_frequency;

static int PrintLinesOn;
static int SensorInputsOn;

static int SafeModeOn;

static int UID;

//This signals that the grid is critical AND sheds the UIDs load
//These could be different functions.
static void ShedLoad(int targetUID){

    NodalMessage gridCriticalMessage;
    gridCriticalMessage.targetUID=targetUID;
    gridCriticalMessage.messageIdentifier = SIGNAL_GRID_CRITICAL;
    gridCriticalMessage.msgType=int_message;
    gridCriticalMessage.intData=-1;
    gridCriticalMessage.originatingUID=UID;

    SendNodalMessage(&gridCriticalMessage);

    NodalMessage dispatchMsg;
    dispatchMsg.targetUID = targetUID;
    dispatchMsg.messageIdentifier = TURN_OFF_PORTS_DISPATCH_MESSAGE;    //1 means it's a dispatching message
    dispatchMsg.msgType = int_message;
    dispatchMsg.intData = -1;
    dispatchMsg.originatingUID=UID;

    SendNodalMessage(&dispatchMsg);

    //Do the book keeping for our local state
    updateGridStatePorts(targetUID,0);
    updateGridState(targetUID,0);

}

static void ShedIndividualLoad(){

    int UIDOfLoadToBeShed = getTargetUIDOfMaxActivityNode();
    
    if(UIDOfLoadToBeShed > 0){
        ShedLoad(UIDOfLoadToBeShed);
    }
}

//Like ShedLoad, this also signals that the grid is OK
//Could be divided into two different functions 
static void TurnNodeOn(int targetUID){

    NodalMessage gridNonCriticalMessage;
    gridNonCriticalMessage.targetUID=targetUID;
    gridNonCriticalMessage.messageIdentifier = SIGNAL_GRID_NON_CRITICAL;
    gridNonCriticalMessage.msgType=int_message;
    gridNonCriticalMessage.intData=-1;
    gridNonCriticalMessage.originatingUID=UID;

    SendNodalMessage(&gridNonCriticalMessage);
    
    NodalMessage dispatchMsg;
    dispatchMsg.targetUID = targetUID;
    dispatchMsg.messageIdentifier = TURN_ON_PORTS_DISPATCH_MESSAGE;    //1 means it's a dispatching message
    dispatchMsg.msgType = int_message;
    dispatchMsg.intData = -1;
    dispatchMsg.originatingUID=UID;

    SendNodalMessage(&dispatchMsg);
}

static void  TurnIndividualLoadOn(){
    int UIDofLoadTurnedOn = getTargetUIDOfMinActivityNode();
    if(UIDofLoadTurnedOn>0){
        TurnNodeOn(UIDofLoadTurnedOn);
    }
}

//Will shed all load
static void DispatchSafeMode(){

    SafeModeOn=1;
    int ** connectedUIDs = getConnectedUIDs();
    int numNodes = getNumberOfConnectedNodes();
    int i;
    for(i=0;i<numNodes;i++){        
        ShedLoad((*connectedUIDs)[i]);
        updateGridStatePorts((*connectedUIDs)[i],0);
    }
}

static void ExitSafeMode(){
    SafeModeOn=0;
    int ** connectedUIDs = getConnectedUIDs();
    int numNodes = getNumberOfConnectedNodes();
    int i;
    for(i=0;i<numNodes;i++){
        //Makes the assumption UIDs are 0 to 4
        TurnNodeOn((*connectedUIDs)[i]);
        updateGridStatePorts((*connectedUIDs)[i],1);
    }
}

//This determines if we can serve the remaining loads for the day given our current
//battery state and number of loads and the energy minimum we need
//We assume that lights are critical and that we need to have enough energy for them
static int checkEnergySatisfiability(){

    float energyRequiredForLighting = getExpectedLightingEnergy();      //This is in W*hr
    float expectedRemainingDemand = getExpectedRemainingDemand();
    float currentBatteryEnergy = getBatteryEnergy();
    float remainingSolarEnergy = getExpectedRemainingDailySolarEnergy();

    int satisfiability=0;
    //If this is met, we are good and do not need to shed load
    if(currentBatteryEnergy + remainingSolarEnergy > energyRequiredForLighting +expectedRemainingDemand +MINIMUM_BATTERY_ENERGY){
        satisfiability = 1;
    }
    return satisfiability;
}

//To determine if we need to shed any loads
static void DetermineContinuedDispatch(){   

    float currentBatteryEnergy = getBatteryEnergy();

    //If this runs, we are CRITICAL
    if(currentBatteryEnergy < MINIMUM_BATTERY_ENERGY){
            
            DispatchSafeMode();

    //If this runs , we are NON-CRITICAL
    }else if(currentBatteryEnergy > NON_CRITICAL_ENERGY_LEVEL && isGridInSafeMode()){
           ExitSafeMode();

    //Here we are somewhere in between
    }else{

        int doWeHaveEnoughEnergy = checkEnergySatisfiability();
        //Debugging Line
        if(!SensorInputsOn) doWeHaveEnoughEnergy = 1;

        //This runs if we don't have enough energy and need to start shedding loads
        if(!doWeHaveEnoughEnergy){
            int numberOfConsumingNodes = getNumberOfNodesCurrentlyConsuming();

            while(!doWeHaveEnoughEnergy &&  numberOfConsumingNodes > 0){
                doWeHaveEnoughEnergy = checkEnergySatisfiability();
                ShedIndividualLoad();
                numberOfConsumingNodes--;
            }

        }else{            
            int numberOfNodesTurnedOff = getNumberOfNodesShed();

            while(numberOfNodesTurnedOff > 0 && doWeHaveEnoughEnergy){
                TurnIndividualLoadOn();
                doWeHaveEnoughEnergy = checkEnergySatisfiability();
                numberOfNodesTurnedOff--;
            }



        }
    }
}

void initDispatcher(long sysFrequency,int debugMode,int myUID){
    UID=myUID;
    SafeModeOn=0;
    PrintLinesOn=0;
    SensorInputsOn=1;

    if(debugMode == 1){
         PrintLinesOn=1;SensorInputsOn=0;
    }else if(debugMode ==2){
        SensorInputsOn=0;
        PrintLinesOn=0;
    }else if(debugMode == 3){
        SensorInputsOn= 1;
        PrintLinesOn = 1;
    }
    
          
    system_frequency = sysFrequency;

    dispatchTimerFlag = 0;

    PR3 = system_frequency/(3*256);
    T3CONbits.TON = 1;
    T3CONbits.TCKPS = 0b11;

    IPC2bits.T3IP = 5;
    IFS0bits.T3IF = 0;
    IEC0bits.T3IE = 1;

    register_computer_message_handler(PUT_SOLAR_FORECASTS,
                                      solar_forecast_upload_handle);
}


void DetermineDispatch(int requestingUID, float requestedEnergy){

    
    int dispatchable = 0; //The golden variable    


    float expectedRemainingSolarEnergy;
    float expectedRemainingDemand;
    float remainingBatteryEnergy;

    if(SensorInputsOn){
        expectedRemainingSolarEnergy = getExpectedRemainingDailySolarEnergy();
        expectedRemainingDemand = getExpectedRemainingDemand();
        remainingBatteryEnergy = getBatteryEnergy();
    }else{
        expectedRemainingSolarEnergy = 1200;
        expectedRemainingDemand = 50;
        remainingBatteryEnergy = 1200;
    }

    //For now, the simplest way possible to do this is to look at the
    //expected energy available and see how much expected demand you have
    //and then make a decision based on some safety margin 
    float energySafetyMargin = 0.25*getTotalBatteryEnergy();

    //The line that makes the decision
    if( expectedRemainingSolarEnergy + remainingBatteryEnergy  - requestedEnergy > energySafetyMargin){
    //if( expectedRemainingSolarEnergy + remainingBatteryEnergy  - requestedEnergy - expectedRemainingDemand > energySafetyMargin){
    //if(remainingBatteryEnergy > 0){
    //We are dispatching it
        dispatchable = 1;
        //SendComputerFloat(requestedEnergy);
        updateGridState(requestingUID,requestedEnergy);
    }else{
        //We are not dispatching it, in fact, we are shutting it all off!
        ShedLoad(requestingUID);
        updateGridStatePorts(requestingUID,0);
    }
}

int isGridInSafeMode(){
    return SafeModeOn;
}

void __attribute__((__interrupt__,auto_psv)) _T3Interrupt(void){

    if(dispatchTimerFlag >= CONTINUED_DISPATCH_FLAG_TIMER){
        DetermineContinuedDispatch();
        dispatchTimerFlag = 0;
    }else{
        dispatchTimerFlag++;
    }

     IFS0bits.T3IF = 0;
}
