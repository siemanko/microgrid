/* 
 * File:   GridStateManagement.h
 * Author: Dan
 *
 * Created on December 20, 2014, 4:25 PM
 */

#ifndef GRIDSTATEMANAGEMENT_H
#define	GRIDSTATEMANAGEMENT_H

typedef struct activity{
    int startTime;
    int endTime;
    float power;
} Activity;

typedef struct nodeStruct{
    int UID;
    char type;
    int port1;                     //1 if port 1 is consuming
    int port2;                     //1 if port 2 is consuming
    int pwm_light;                 //1 if pwm light is consuming
    float powerConsumption;
    int nodeShed;
    int port1SwitchedOn;        //Determines if load shedder switched this off
    int port2SwitchedOn;        //Determines if load shedder switched this off
    int lightPortSwitchedOn;    //Determines if load shedder switched this off
    float totalEnergyReceived;
    short totalActivitiesDispatched;
   // Activity act1;   //Could include an activity here
} Node;

void initializeGridState();
void initializeState(int myUID, char myType,int debugMode);

void addGridNode(int newUID);
void updateGridState(int updatedUID,float powerUpdate);
void updateGridStatePorts(int shedUID, int onOROff);

float getExpectedRemainingPowerDemanded();
int getNumberOfConnectedNodes();
int ** getConnectedUIDs();
Node ** getConnectedNodes();

void setABoxUID(char ABox_in);
int getABoxUID();
int amIConnectedToTheNetwork();
void connectBoxToTheNetwork();
void disconnectBoxFromTheNetwork();

void regiserBBoxOnTheNetwork(int UID_B_Box);
int getNodeUIDAtIndex(int index);
void removeNodeFromGridState(int indexOfRemovedUID);
int debuggingGetUIDOfFirstElement();
Node * getThisNode();
char getMyUID();

// Used for .
void state_of_grid_handler(char* ignore);
int getTargetUIDOfMaxActivityNode();
int getTargetUIDOfMinActivityNode();
int getNumberOfNodesCurrentlyConsuming();
int getNumberOfNodesShed();

void print_energy_state_handler(char*);

#endif	/* GRIDSTATEMANAGEMENT_H */

