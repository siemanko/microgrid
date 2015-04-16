/* 
 * File:   NetworkCommConstants.h
 * Author: Dan
 *
 * Created on January 2, 2015, 2:14 AM
 */

//THE FOLLOWING ARE TYPES OF MESSAGES SENT
#define PLUG_IN_EVENT_MSG                       2
#define UNPLUG_EVENT_MSG                        3
#define TURN_OFF_PORTS_DISPATCH_MESSAGE         4
#define TURN_ON_PORTS_DISPATCH_MESSAGE         20
#define SIGNAL_GRID_CRITICAL                   21
#define SIGNAL_GRID_NON_CRITICAL               22

#define PASS_TOKEN_MESSAGE_TYPE                 5
#define NODE_DISCOVERY_MESSAGE_TYPE             8
#define ANNOUNCE_NODE_ON_THE_NETWORK            9
#define CONFIRM_BOX_CONNECTED_TO_NETWORK_TYPE   10
#define ACK_RECEIPT_OF_TOKEN_TYPE               11

#define NODE_DISCOVERY_MESSAGE_TARGET_UID       59

#define PING_NEW_NODES_DELAY_TIMER              1
#define ACKNOWLEDGEMENT_DELAY_TIMER             5

#define END_OF_MESSAGE_SYMBOL                   37

#define END_OF_TRANSMISSION_1                   96
#define END_OF_TRANSMISSION_2                   51

#define B_BOX_TIMEOUT_TIMER                     10

#define TEST_MESSAGE                            100


//Assume we only send 1 int per message
//Format:  [Target_Address, LEN, Identifier, Int Data, Originating UID, EOM ]
#define SIZE_OF_INT_MESSAGE                     5

//Assume weo nly send 1 float per message
//Format:   [Target_Address, LEN, Identifier, Float Data, Originating UID, EOM ]
#define SIZE_OF_FLOAT_MESSAGE                   8

//Assume weo nly send 1 float per message
//Format:   [Target_Address, LEN, Identifier, Float Data, Int Data, Originating UID, EOM ]
#define SIZE_OF_FLOAT_INT_MESSAGE               10

//When someone wants to reach the entire network, all will accept this UID
//So no one node can use 99 as UID
#define BROADCAST_UID                           99