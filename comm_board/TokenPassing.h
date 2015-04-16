/* 
 * File:   TokenPassing.h
 * Author: sidor
 *
 * Created on January 22, 2015, 2:23 AM
 */

#ifndef TOKENPASSING_H
#define	TOKENPASSING_H

#include "NodalCommunicationDriver.h"

#define TP_MODE_SLAVE                         0
#define TP_MODE_MASTER                        1

//#define uint8_t unsigned char

typedef enum EnumMasterState {
    MASTER_UNJAMMING,
    MASTER_CONNECTIONS,
    MASTER_AWAITING_CONNECTIONS,
    MASTER_IHAZTOKEN,
    MASTER_TOKEN_OUT,
    MASTER_TOKEN_IN,
} MasterState;

typedef enum StructSlaveState {
    SLAVE_DISCONNECTED,
    SLAVE_CONNECTED
} SlaveState;

void initTokenPassing(int as_master);
void tokenPassingLogic(void);
int getTPState();
void resetTokenPassing(void);
void on_token_passing_message(NetworkMessage * msg);

void tpPingConnectedNodes();

// if unknown return 255.
uint8_t getMasterUid(void);
int isConnected(void);
#endif	/* TOKENPASSING_H */

