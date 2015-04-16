#include <p33EP512GM710.h>
#include "stdint.h"
#include "string.h"
#include "NodalCommunicationInterface.h"
#include "NodalCommunicationDriver.h"
#include "LoadBoardInterface.h"
#include "DispatchingExecutive.h"
#include "GridStateManagement.h"
#include "LibraryFunctions.h"
#include "ComputerCommunicationDriver.h"
#include "NetworkCommConstants.h"
#include "UserLEDs.h"
#include "debugging.h"

static int currentIndexOfNodeWithToken;

static int UID;
static char typeOfBox;

static int pingNewNodesTimer;
static int acknowledgementTimer;

static int BBoxNetworkTimeoutTimer;

static int roundsOfInitialization;      //The variable that causes them to sign on at different times
static int totalRoundsOfInitialization;

static int debug_mode;


static int PrintOutsOnDebugging;
static int LoadBoardsOnDebugging;

static int PassTheTokenTimer2;
static int PassTheTokenUID;


static void delayDebug(float cycles){
    int i;int j;int k;
    //int p=0;
    for(i = 0;i<cycles;i++){
        for(j=0;j<cycles;j++){
            for(k=0;k<100;k++){
           }
        }
    }
}

static float raw2ieee(uint8_t raw[]){
    // either
    union {
        uint8_t bytes[4];
        float fp;
    } un;
    memcpy(un.bytes, raw, 4);
    return un.fp;
}

static void AttemptNodeDiscovery(){
    /*
    //putUART1(45);
    //if(PrintOutsOnDebugging){  putUART1(45);}
    NodalMessage connectionMsg;
    connectionMsg.msgType = int_message;
    connectionMsg.targetUID = NODE_DISCOVERY_MESSAGE_TARGET_UID;
    connectionMsg.messageIdentifier = NODE_DISCOVERY_MESSAGE_TYPE;
    connectionMsg.intData = -1;
    connectionMsg.originatingUID = UID;

    SendNodalMessage(&connectionMsg);
    flushFIFOSendBufferU2();   //Actually send the messages once we have the token  

     */
}

static void AcknowledgeReceiptOfToken(){
/*
    NodalMessage ackMsg;
    ackMsg.targetUID = getABoxUID();
    ackMsg.msgType = int_message;
    ackMsg.messageIdentifier = ACK_RECEIPT_OF_TOKEN_TYPE;
    ackMsg.originatingUID = UID;
    ackMsg.intData = -1;

    SendNodalMessage(&ackMsg);
 */
}

static void PassTheToken(){
    /*
    if(PrintOutsOnDebugging){ putUART1(70);}
    NodalMessage passTheTokenMessage;

    if(typeOfBox=='A'){
        passTheTokenMessage.targetUID = getNodeUIDAtIndex(currentIndexOfNodeWithToken);
        currentIndexOfNodeWithToken++;
        if(currentIndexOfNodeWithToken>=getNumberOfConnectedNodes()) currentIndexOfNodeWithToken = 1; //Starting a new round, but skipping the A box     
        //putUART1(86);
        //putUART1(passTheTokenMessage.targetUID);
        T6CONbits.TON = 1;      //Turn on the ack timer
        
    }else{

        passTheTokenMessage.targetUID = getABoxUID();
    }
    passTheTokenMessage.messageIdentifier = PASS_TOKEN_MESSAGE_TYPE;
    passTheTokenMessage.originatingUID = UID;
    passTheTokenMessage.msgType = int_message;
    passTheTokenMessage.intData = -1;


    SendNodalMessage(&passTheTokenMessage);
    
    flushFIFOSendBufferU2();   //Actually send the messages once we have the token

     */
}

static void receiveToken(){
    /*
    if(PrintOutsOnDebugging) putUART1(104);
    //putUART1(105);
    
    //Only B's need to acknowledge receipt of token
    if(typeOfBox=='B') AcknowledgeReceiptOfToken();
   
    flushFIFOSendBufferU2();    //Send any messages that might have built up

    //delayDebug(5);
    //Now we poll the network to see if there are any new nodes
    //There will be a timeout if there are no new node
    if(typeOfBox=='A'){
        //Assumes that the A box is always the first in the grid state list        
        AttemptNodeDiscovery();
        T5CONbits.TON = 1;      //Will timeout if no-one found
    }else if(typeOfBox=='B'){
        PassTheToken();
    }    */
}

static void attemptConnectionToNetwork(){
    /*if(PrintOutsOnDebugging){ putUART1(113);}
    NodalMessage connectionMsg;
    connectionMsg.targetUID = getABoxUID();
    connectionMsg.messageIdentifier = ANNOUNCE_NODE_ON_THE_NETWORK;
    connectionMsg.intData = -1;
    connectionMsg.originatingUID = UID;

    SendNodalMessage(&connectionMsg);
    
    flushFIFOSendBufferU2();   //Actually send the messages once we have the token

    */
}

static void acknowledgeNewNodeOnNetwork(int newNodeUID){ 
/*
    NodalMessage newNodeAck;
    newNodeAck.targetUID = newNodeUID;
    newNodeAck.messageIdentifier = CONFIRM_BOX_CONNECTED_TO_NETWORK_TYPE;
    newNodeAck.intData = - 1;
    newNodeAck.originatingUID = UID;
    newNodeAck.msgType = int_message;
  
    SendNodalMessage(&newNodeAck);*/
}

//Will be called if we fail to hear back from a node
static void removeNode(){
   /* removeNodeFromGridState(currentIndexOfNodeWithToken);
    if(getNumberOfConnectedNodes() ==1){
        AttemptNodeDiscovery();
        T5CONbits.TON = 1;
    }else{
        PassTheToken();
    }*/
}

//Without Data Field
static int computeCheckSum(NodalMessage * msg){
  /*  int sum = 0;
    sum+=msg->targetUID;
    sum+=msg->messageIdentifier;
    sum+=msg->originatingUID;
    
    return sum;*/
}

//1 if true, 0 if incorrect
//Currently, this is only implemented for integer message types
//of format [targetUID, len, type, data, originating UID, CheckSum]
static int checkCheckSum(char ** msg){
/*
    int indexOfCheckSum = 3;    //Assuming it's at the end of message for htis type of message
    int incomingCheckSum = (*msg)[indexOfCheckSum];
    
    //For now, checksum will only contain targetUID, originatingUID, and Message Type
    int incomingCheckSumCheck = (*msg)[0]+(*msg)[2]+(*msg)[4];

    int checkSumTrue = 0;
    if(incomingCheckSum == incomingCheckSumCheck) checkSumTrue = 1;

    return checkSumTrue;*/
}


void initNodalCommunicationInterface(int UID_in, char type_in,long sys_frequency,int debugMode){
    /*UID = UID_in; typeOfBox = type_in;debug_mode=debugMode;
    PrintOutsOnDebugging=0;
    LoadBoardsOnDebugging=0;
    if(debug_mode == 0 || debug_mode == 3) LoadBoardsOnDebugging = 1;
    if(debug_mode == 2 || debug_mode == 3) PrintOutsOnDebugging = 1;

    currentIndexOfNodeWithToken = 1;
    roundsOfInitialization = 0;
    BBoxNetworkTimeoutTimer=0;
    totalRoundsOfInitialization=UID;
    
    initNetworkCommunication(UID);
    
    //Ensuring that whoever is UID = 1 starts the token passing
    if(typeOfBox=='A'){
         //Initialize Time-Out Timer        
         pingNewNodesTimer = 0;

        //This is the exploratory message timer
         PR5 = sys_frequency/(3*256);
         T5CONbits.TON = 1;
         T5CONbits.TCKPS = 0b11;

         IPC7bits.T5IP = 3;
         IFS1bits.T5IF = 0;
         IEC1bits.T5IE = 1;

         acknowledgementTimer=0;
    }
         //This is the message time-out timer
    PR6 = sys_frequency/(3*256);
    T6CONbits.TON = 0;
    T6CONbits.TCKPS = 0b11;
    if(typeOfBox == 'B') T6CONbits.TON = 1;
    IPC11bits.T6IP = 3;
    IFS2bits.T6IF = 0;
    IEC2bits.T6IE = 1;


    register_computer_message_handler(GET_NODES_CONNECTED_TO_NETWORK,
                                      get_connected_nodes_handler);

*/
}

//green is ground

//data is red

void SendNodalMessage(NodalMessage * msg){
    /*debug(DEBUG_NETWORK, "SENDING MSG TO %d", msg->targetUID);
     //Default to int message as type
    if(msg->msgType == float_message){
        putCharNodalCommSendFIFO(SIZE_OF_FLOAT_MESSAGE);
    } else if (msg->msgType == int_message) {
        putCharNodalCommSendFIFO(SIZE_OF_INT_MESSAGE);
    } else if (msg->msgType == float_int_message) {
        putCharNodalCommSendFIFO(SIZE_OF_FLOAT_INT_MESSAGE);
    }

    putCharNodalCommSendFIFO( msg->messageIdentifier);

    //Default to int message as type
    if(msg->msgType == float_message){       
        putFloatNodalCommSendFIFO(&msg->floatData);
    }else if (msg->msgType == int_message) {
         //To start out with, only do checksums for announce node types where it takes place of data field
        if( msg->messageIdentifier == ANNOUNCE_NODE_ON_THE_NETWORK){
            putCharNodalCommSendFIFO(computeCheckSum(msg));
        }else{
            putCharNodalCommSendFIFO(msg->intData);
        }        
    } else if (msg->msgType == float_int_message) {
        putFloatNodalCommSendFIFO(&msg->floatData);
        putIntNodalCommSendFIFO(&msg->intData);
    }

    putCharNodalCommSendFIFO( msg->originatingUID);
    putCharNodalCommSendFIFO(END_OF_MESSAGE_SYMBOL);*/
    
}

//Note for future:  This receive message function should contain error
//checks on the messages, such as the length of the message being expected, etc. etc.
//Assumes format  [  Target_Address_UID, LEN, Type, Data, Originating Address, EOM]
void ProcessNodalMessage(char * message){
    int len = message[1];
    debug(DEBUG_NETWORK, "Got message from %d", (int)message[len-1]);
    //Switch on the message type (which is the second field of the message)
    switch (message[2]){
   
        case PLUG_IN_EVENT_MSG: ;
            //putUART1(69);
            //Convert message bytes to float
            uint8_t dataIn[4];
            int j;
            int offset = 6;   //Message format: [  Target_Address_UID (0), LEN (1), Type (2), Float Data (3-6), Originating Address (7), EOM]
            for(j=3;j>=0;j--) dataIn[j] = message[offset-j];
            float currentDemand = raw2ieee(dataIn);
            int balance = BytesToInt(&(message[7]));
            debug(DEBUG_NETWORK, "Balance: %d", balance);
            //Determine dispatch passed originating UID and energy requested
            //This could be expanded to include other fields            
            DetermineDispatch( message[7] , currentDemand);
            break;

        case UNPLUG_EVENT_MSG: ;            
            int originatingUID = message[7];
            uint8_t dataIn2[4];
            int k;
            int offset2 = 6;   //Message format: [  Target_Address_UID (0), LEN (1), Type (2), Float Data (3-6), Originating Address (7), EOM]
            for(k=3;k>=0;k--) dataIn[k] = message[offset2-k];
            float currentDemand2 = raw2ieee(dataIn2);

            updateGridState(originatingUID,currentDemand2);
            break;

        case TURN_OFF_PORTS_DISPATCH_MESSAGE:
            // turnOffAllLoadBoardPorts();
            break;

        case TURN_ON_PORTS_DISPATCH_MESSAGE:
            break;

        case PASS_TOKEN_MESSAGE_TYPE:            
            receiveToken();
            break;

        case NODE_DISCOVERY_MESSAGE_TYPE:
            
            if(!amIConnectedToTheNetwork() && roundsOfInitialization==totalRoundsOfInitialization){
                setABoxUID(message[4]);
                attemptConnectionToNetwork();
            }else if(!amIConnectedToTheNetwork()){
                roundsOfInitialization++;
            }
            break;

        case ANNOUNCE_NODE_ON_THE_NETWORK:  //If a type A box has received a msg from a new node
            if(checkCheckSum(&message)){
                T5CONbits.TON = 0; //Turn off Timer that would be waiting for this message
                int newNodeUID = message[4];
                addGridNode(newNodeUID);     //Assuming that only one A box will be registering B boxes
                acknowledgeNewNodeOnNetwork(newNodeUID);
                PassTheToken();         //And then end it
            }

            break;
            
        case CONFIRM_BOX_CONNECTED_TO_NETWORK_TYPE:
            connectBoxToTheNetwork();            
            break;

        case ACK_RECEIPT_OF_TOKEN_TYPE:
           
            T6CONbits.TON = 0;
            acknowledgementTimer=0;
            break;

        case SIGNAL_GRID_NON_CRITICAL:
            signalGridNotCritical();
            break;

        case SIGNAL_GRID_CRITICAL:
            signalGridCritical();
            break;
        case TEST_MESSAGE:
            LCD_print("Received ", 1);
            break;
        default:
            break;
    }
}

//This happens whenever the boxes receive any message and tells them that they are still connected to the network.
//If it does not run, the box will disconnect itself.
void clearMessageReceivedTimer(){
    //if(PrintOutsOnDebugging) putUART1(34);
    //BBoxNetworkTimeoutTimer=0;
}

void __attribute__((__interrupt__,auto_psv)) _T5Interrupt(void){  
/*
  
    if(pingNewNodesTimer >= PING_NEW_NODES_DELAY_TIMER){
        pingNewNodesTimer=0;

        //If there are no other nodes on the network, we need to discover them
        //There is no reason to pass the token
        if(getNumberOfConnectedNodes() == 1){
            AttemptNodeDiscovery();
        }else{
            PassTheToken();
        }

    }else{
        pingNewNodesTimer++;
    }

    IFS1bits.T5IF = 0;*/
}


//This is an interrupt which acts in two ways, depending on which type of box
//you are.  First, if you are an A box, it is the timeout that controls whether you think
//a certain B box is connected to the network. Second, if you are a B box, it controls whether
//You think you are still connected to the network.
void __attribute__((__interrupt__,auto_psv)) _T6Interrupt(void){
/*
    if(typeOfBox=='A'){
        if(acknowledgementTimer > ACKNOWLEDGEMENT_DELAY_TIMER){
            acknowledgementTimer=0;
            removeNode();
            T6CONbits.TON = 0;
        }else{
            acknowledgementTimer++;
        }

    }else if(typeOfBox == 'B'){
       
        if(BBoxNetworkTimeoutTimer >= B_BOX_TIMEOUT_TIMER){
            BBoxNetworkTimeoutTimer=0;
            disconnectBoxFromTheNetwork();
            turnOffAllLoadBoardPorts();            
        }else{
            BBoxNetworkTimeoutTimer++;
        }
    }

    IFS2bits.T6IF = 0;*/
}

void get_connected_nodes_handler(char* ignored) {
   /* int numNodes = getNumberOfConnectedNodes();
    int ** connectedUIDs = getConnectedUIDs();
    int i;

    debug(DEBUG_NETWORK, "%d nodes connected", numNodes);
    for(i =0;i<numNodes;i++){
        int uid = (*connectedUIDs)[i];
        debug(DEBUG_NETWORK, "%d", uid);
    }*/
}
