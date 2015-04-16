#include "GridStateManagement.h"
#include "stdlib.h"
#include "LoadBoardConstants.h"
#include "ComputerCommunicationDriver.h"
#include "ComputerMessages.h"
#include "debugging.h"
#include "BatteryManagement.h"
#include "UserManagement.h"

#define MAX_NUMBER_OF_NODES             5

static Node thisNode;           //Node holding the local state

static Node * nodes;
static int * listOfUIDs;
static int numberOfConnectedNodes;
static int max_size_of_array;

static int ABoxUID; //Assumes that we only have one A Box
static int isConnectedToNetwork;

static int PrintLinesOn;

void initializeGridState(){
    //We allocate too much so we are not always ralloc'ing
    nodes = (Node *)malloc(MAX_NUMBER_OF_NODES*sizeof(Node));
    listOfUIDs = (int *)malloc(MAX_NUMBER_OF_NODES*sizeof(int));

    //Assuming that this node is always the first element of the node arrays
    //This assumption is used in other parts of the code
    nodes[0] = thisNode;
    listOfUIDs[0] = thisNode.UID;
    
    numberOfConnectedNodes = 1;

    if(thisNode.type == 'A'){
        ABoxUID = thisNode.UID;
        isConnectedToNetwork = 1;
    }else{
        ABoxUID=0;
        isConnectedToNetwork = 0;
    }
    max_size_of_array = MAX_NUMBER_OF_NODES;

    register_computer_message_handler(GET_STATE_OF_GRID, state_of_grid_handler);
    register_computer_message_handler(GET_ENERGY_STATE,
                                      print_energy_state_handler);
}

//Initializes the local state and then the network state
void initializeState(int myUID, char myType, int debugMode){

    PrintLinesOn=0;
    if(debugMode) PrintLinesOn=1;

    thisNode.UID = myUID;
    thisNode.type = myType;

    thisNode.port1=0;
    thisNode.port2=0;
    thisNode.pwm_light=0;
    thisNode.powerConsumption = 0;
    thisNode.lightPortSwitchedOn=1;
    thisNode.port1SwitchedOn=1;
    thisNode.port2SwitchedOn=1;
    thisNode.nodeShed=0;
    thisNode.totalActivitiesDispatched=0;
    thisNode.totalEnergyReceived = 0;



    initializeGridState();
}

void addGridNode(int newUID){
    numberOfConnectedNodes++;
    Node newNode; //{newUID, 'B'};     //For now, all nodes we will add will be B boxes
    newNode.UID=newUID;
    newNode.type = 'B';
    newNode.port1=0;
    newNode.port2=0;
    newNode.pwm_light = 0;
    newNode.powerConsumption = 0;
    newNode.lightPortSwitchedOn = 1;
    newNode.port1SwitchedOn = 1;
    newNode.port2SwitchedOn = 1;
    newNode.nodeShed = 0;
    newNode.totalActivitiesDispatched = 0;
    newNode.totalEnergyReceived = 0;

    if(numberOfConnectedNodes >= max_size_of_array){
        max_size_of_array*=2;
        nodes = (Node *)realloc(nodes,(max_size_of_array)*sizeof(Node));
        listOfUIDs = (int *)realloc(listOfUIDs, max_size_of_array*sizeof(int));        
    }

    

    nodes[numberOfConnectedNodes-1]=newNode;
    listOfUIDs[numberOfConnectedNodes-1] = newUID;



}


//This updates the grid state and whether something is turned on or off
//The message encoding uses the same as the load board messages
void updateGridState(int updatedUID,float power){
    //First, find the index of the node that we want to update
    int index = -1;
    int i;
    for(i = 0; i < numberOfConnectedNodes;i++){
        if(listOfUIDs[i]==updatedUID){
            index=i;
            //break;
        }
    }

    //Error check
    if(index >=0 && index < numberOfConnectedNodes){
        nodes[index].totalEnergyReceived+=power;
        //Only increase the number of activities if you are asking for more power (i.e. not if you unplugged something)        
        if(nodes[index].powerConsumption < power) nodes[index].totalActivitiesDispatched++;
        nodes[index].powerConsumption = power;

    }
   

    //This code makes no sense really becaues we can't tell which port is on/off
    //Might try to include it later
    /*
    //Error check
    //Note that the situation may arise where we are updating the list of UIDs and
    //(before we can fully update), someone unplugs their box and we will get an array error
    //and everything will crash.  It seems like there's a small probability of this happening, but
    //might want ot be considered for the future.
    if(index >=0 && index < numberOfConnectedNodes){
        switch(command){
            case TURN_LIGHT_ON_MSG:
                (*Node)[index].pwm_light = 1;
                break;
            case TURN_LIGHT_OFF_MSG:
                (*Node)[index].pwm_light=0;
                break;
            case TURN_PORT1_ON_MSG:
                (*Node)[index].port1=1;
                break;
            case TURN_PORT1_OFF_MSG:
                (*Node)[index].port1=0;
                break;
            case TURN_PORT2_ON_MSG:
                (*Node)[index].port2=1;
                break;
            case TURN_PORT2_OFF_MSG:
                (*Node)[index].port2=0;
                break;
            default:
                break;
       };        
    }
     */
}

void updateGridStatePorts(int shedUID,int OnOrOff){
    //First, find the index of the node that we want to update
    int index = -1;
    int i;
    for(i = 0; i < numberOfConnectedNodes;i++){
        if(listOfUIDs[i]==shedUID){
            index=i;
            break;
        }
    }
    //Error check
    if(index >=0 && index < numberOfConnectedNodes){
        nodes[index].port1SwitchedOn=OnOrOff;     //Switch them on or off
        nodes[index].port2SwitchedOn=OnOrOff;
        nodes[index].lightPortSwitchedOn=OnOrOff;
        nodes[index].nodeShed = 1-OnOrOff;
    }    
}

int getNumberOfConnectedNodes(){
    return numberOfConnectedNodes;
}

int ** getConnectedUIDs(){
    return &listOfUIDs;
}

Node ** getConnectedNodes(){
    return &nodes;
}

void setABoxUID(char ABox_in){
    ABoxUID = ABox_in;
}

int getABoxUID(){
    return ABoxUID;
}

int amIConnectedToTheNetwork(){
    return isConnectedToNetwork;
}

void connectBoxToTheNetwork(){
    isConnectedToNetwork = 1;
}

void disconnectBoxFromTheNetwork(){
    isConnectedToNetwork = 0;
}

int getNodeUIDAtIndex(int index){
    int result = -1;
    //Check that we are asking for an index in the array    
    if(index < numberOfConnectedNodes){
        result = listOfUIDs[index];
    }
    return result;
}

void removeNodeFromGridState(int indexOfRemovedUID){
    numberOfConnectedNodes--;
    int i;
    for(i=indexOfRemovedUID;i<numberOfConnectedNodes;i++){
        nodes[i] = nodes[i+1];
        listOfUIDs[i] = listOfUIDs[i+1];
    }
}

int debuggingGetUIDOfFirstElement(){
    return listOfUIDs[0];
}

Node * getThisNode(){
    return &thisNode;
}

char getMyUID(){
    return thisNode.UID;
}

int getTargetUIDOfMaxActivityNode(){
    int curUID=-1;
    int curMax = 0;
    int i;
    for(i=0;i<numberOfConnectedNodes;i++){
	if(nodes[i].powerConsumption > 0.01 && nodes[i].totalActivitiesDispatched > curMax){
        	curUID = nodes[i].UID;
		curMax=nodes[i].totalActivitiesDispatched;
	}
    }
    return curUID;
}

int getTargetUIDOfMinActivityNode(){
    int curUID;
    unsigned int curMin = 150;
    int i;
    for(i=0;i<numberOfConnectedNodes;i++){
        if(nodes[i].nodeShed == 1 && nodes[i].totalActivitiesDispatched < curMin){
            curUID = nodes[i].UID;
            curMin = nodes[i].totalActivitiesDispatched;
        }
    }
    return curUID;
}

int getNumberOfNodesCurrentlyConsuming(){
    int numberOfNodes=0;
    int i;
    for(i=0;i<numberOfConnectedNodes;i++){
        if(nodes[i].powerConsumption > 0.01){
            numberOfNodes++;
        }
    }
    return numberOfNodes;
}

int getNumberOfNodesShed(){
    int numberOfNodes=0;
    int i;
    for(i=0;i<numberOfConnectedNodes;i++){
        if(nodes[i].nodeShed == 1) numberOfNodes++;
    }
    return numberOfNodes;
}

void state_of_grid_handler(char* ignore){
    Node ** nodes = getConnectedNodes();
    int numberOfConnectedNodes = getNumberOfConnectedNodes();

    //How much data the computer needs to read
    int i;

    debug(DEBUG_GRID_STATE, "Number of nodes: %d", numberOfConnectedNodes);

    for(i=0;i<numberOfConnectedNodes;i++){
       debug(DEBUG_GRID_STATE,
             "UID(%d): consumption: %f, activites: %d, shed: %d",
             (*nodes)[i].UID,
             (*nodes)[i].powerConsumption,
             (*nodes)[i].totalActivitiesDispatched,
             (*nodes)[i].nodeShed);
    }
}


void print_energy_state_handler(char* ignored) {

    float totalBatteryEnergy = getTotalBatteryEnergy();
    float remainingSolarEnergy = getExpectedRemainingDailySolarEnergy();

    debug(DEBUG_GRID_STATE, "Total battery energy: %f",totalBatteryEnergy);
    debug(DEBUG_GRID_STATE, "Expecetd remaining solar: %f", remainingSolarEnergy);

}