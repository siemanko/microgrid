#include <p33EP512GM710.h>
#include <string.h>
#include <stdlib.h>

#include "lcd.h"
#include "NodalCommunicationDriver.h"
#include "ComputerCommunicationDriver.h"
#include "NetworkCommConstants.h"
#include "circular_buffer.h"
#include "debugging.h"
#include "ComputerMessages.h"
#include "timer.h"
#include "NodalCommunicationInterface2.h"
#include "LibraryFunctions.h"
#include "NodalCommunicationInterface.h"
#include "TokenPassing.h"

#define FP 36850000
#define BAUDRATE 56700
#define BRGVAL ((FP/BAUDRATE)/16)-1

#define MAX_MESSAGE_LENGTH              120

#define NODAL_MAX_INCOMING              64
#define NODAL_MAX_OUTGOING              64

#define UNJAMMING_SYMBOL                69
#define UNJAMMING_END_SYMBOL            70
#define UNJAMMING_THRESHOLD             50

//Used for the input receive character buffer
static char nodal_receive_buffer[MAX_MESSAGE_LENGTH];
static char nodal_receive_next = 0;
static char nodal_receive_total_length = 0;

int nodal_unjamming_counter = 0;

CircularBuffer nodal_incoming;
CircularBuffer nodal_outgoing;



static void initUART2(){
    TRISCbits.TRISC7 = 0;   //DRIVE ENABLE AS OUTPUT, C7, PIN78
    LATCbits.LATC7 = 0;     //Turn enable as low (to begin)

    ANSELBbits.ANSB9 = 0;

    TRISBbits.TRISB9 = 1;   //PIN 76, RB9 as INPUT
    TRISCbits.TRISC6 = 0;   //PIN 77, RC6 as OUTPUT

    RPINR19bits.U2RXR = 41;  //RPI 41, PIN 76, as U2RX
    RPOR6bits.RP54R = 3; //RP54, PIN 77, aS U2TX

    U2MODEbits.STSEL = 0; // 1-Stop bit
    U2MODEbits.PDSEL = 0; // No Parity, 8-Data bits
    U2MODEbits.ABAUD = 0; // Auto-Baud disabled
    U2MODEbits.BRGH = 0; // Standard-Speed mode

    IFS1bits.U2RXIF  = 0;   //U2RX flag set to 0

    IEC1bits.U2RXIE = 1;    //U2RX interrupt enabled
    U2STAbits.URXISEL = 0;//0b11;

    IPC7bits.U2RXIP = 2;    //U2RX interrupt priority
    U2BRG = BRGVAL;

    U2MODEbits.UARTEN = 1; // Enable UART
    U2STAbits.UTXEN = 1; // Enable UART TX

    register_computer_message_handler(GET_NODES_CONNECTED_TO_NETWORK,
                                      connected_nodes);
}


static void enableNodalTransmit(){
    LATCbits.LATC7 = 1;     //DRIVE ENABLE HIGH, C7, PIN78
}

static void enableNodalReceive(){
    LATCbits.LATC7 = 0;
}

static void initNodalReceiveBuffer(void){
    nodal_receive_next = 0;
    nodal_receive_total_length = MAX_MESSAGE_LENGTH;
}

static void putNodalReceiveBuffer(int data){
    if (nodal_receive_next <= MAX_MESSAGE_LENGTH)
        nodal_receive_buffer[nodal_receive_next++] = data;

    if(nodal_receive_next==1 && 0 <= data && data <=MAX_MESSAGE_LENGTH)
        nodal_receive_total_length = data;
}

static void flushNodalReceiveBuffer(void){
    uint8_t total_len = nodal_receive_buffer[0];
    uint8_t target = nodal_receive_buffer[1];
    uint8_t source = nodal_receive_buffer[2];
    uint8_t message_type = nodal_receive_buffer[3];

    // Checking checksum.
    uint32_t cc_computed = checksum(nodal_receive_buffer, total_len-4, 0);
    uint32_t cc_claimed =  BytesToLong(nodal_receive_buffer + total_len-4);

    // remove header
    int len_body = total_len - 8;

    if (cc_computed != cc_claimed) {
        initNodalReceiveBuffer();
        return;
    }
    // Check target
    if (target != getSetting(SETTINGS_UID)
           && target != NODAL_BOARDCAST_ADDRESS) {
        initNodalReceiveBuffer();
        return;
    }
    uint8_t* message_body = (uint8_t*)malloc((len_body+1)*sizeof(uint8_t));
    int i;
    for (i=0; i<len_body; ++i) {
        message_body[i] = nodal_receive_buffer[4+i];
    }
    message_body[len_body] = 0;

    NetworkMessage* msg = (NetworkMessage*)malloc(sizeof(NetworkMessage));
    msg->len = len_body;
    msg->target = target;
    msg->source = source;
    msg->type = message_type;
    msg->body = message_body;

    cb_push(&nodal_incoming, msg);

    initNodalReceiveBuffer();

}


// Computes checksum.
// If header is not zero computes checksum of concatenation of header
// and bytes.
uint32_t checksum(uint8_t * bytes,
                  int length,
                  uint8_t * header) {
    uint32_t res = 0;
    int i;

    if (header != 0) {
        for (i=0; i<4; ++i) {
            res *= 333;
            res += header[i];
        }
    }

    for (i=0; i<length; ++i) {
        res *= 333;
        res += bytes[i];
    }
    return res;
}

void process_system_message(NetworkMessage* msg) {
    // potentially other uses
    if (msg->type == NODALMSG_PING) {
        send_node_mb(msg->source, NODALMSG_PONG, 0);
    } else if (msg->type == NODALMSG_PONG) {
        debug(DEBUG_NETWORK, "Pong from %d", msg->source);
    } else if(msg->type == NODALMSG_TOKEN_IN ||
              msg->type == NODALMSG_TOKEN_OUT ||
              msg->type == NODALMSG_ACCEPTING_CONNECTIONS ||
              msg->type == NODALMSG_CONNECTION_REQUEST) {
        on_token_passing_message(msg);
    }
}


void network_step(void) {
    tokenPassingLogic();
    while(!cb_empty(&nodal_incoming)) {
        NetworkMessage* msg = cb_popqueue(&nodal_incoming);
        if (msg->type < 16) {
            process_system_message(msg);
        } else {
            process_network_message(msg);
        }
        free(msg->body);
        free(msg);
    }
}

void initNetworkCommunication() {
    initUART2();

    initNodalReceiveBuffer();

    enableNodalReceive();

    if (getSetting(SETTINGS_NODE_TYPE) == 'A') {
        initTokenPassing(TP_MODE_MASTER);
    } else {
        initTokenPassing(TP_MODE_SLAVE);
    }

    make_cb(&nodal_outgoing, NODAL_MAX_OUTGOING);
    make_cb(&nodal_incoming, NODAL_MAX_INCOMING);
}

void SendNetworkMessage(NetworkMessage* msg) {
    uint8_t header[4] = {msg->len+8,
                         msg->target,
                         msg->source,
                         msg->type};
    if (0 <= msg->len && msg->len <= MAX_MESSAGE_LENGTH) {
        uint8_t cc_bytes[4];
        LongToBytes(checksum(msg->body, msg->len, header), cc_bytes);
        enableNodalTransmit();
        int i;
        for (i=0; i < 4; ++i) putUART2(header[i]);
        for (i=0; i < msg->len; ++i) putUART2(msg->body[i]);
        for (i=0; i < 4; ++i) putUART2(cc_bytes[i]);
        free(msg->body);
        enableNodalReceive();
    } else {
        debug(DEBUG_NETWORK, "NETWORK MESSAGE BUILD ERROR");
    }
}

void unjamming(void) {
    enableNodalTransmit();

    int i;
    for (i=0; i<2*UNJAMMING_THRESHOLD; ++i) putUART2(UNJAMMING_SYMBOL);
    putUART2(UNJAMMING_END_SYMBOL);

    // unjam myself!
    initNodalReceiveBuffer();
    nodal_unjamming_counter = 0;
    enableNodalReceive();

}

void mb_to_network_message(NetworkMessage* nm,
                           uint8_t node,
                           uint8_t type,
                           MessageBuilder* mb) {
    nm->target = node;
    nm->source = getSetting(SETTINGS_UID);
    nm->type = type;
    if (mb == 0) {
        nm->len = 0;
        nm->body = 0;
    } else {
        nm->len = mb->next_char;
        nm->body = mb->message;
    }
}

void send_node_mb(uint8_t node, uint8_t type, MessageBuilder* mb) {
    NetworkMessage* nm = malloc(sizeof(NetworkMessage));
    mb_to_network_message(nm, node, type, mb);
    cb_push(&nodal_outgoing, nm);
}

void send_node_mb_unsafe(uint8_t node, uint8_t type, MessageBuilder* mb) {
    NetworkMessage nm;
    mb_to_network_message(&nm, node, type, mb);
    SendNetworkMessage(&nm);
}

void SendNetworkMessageRaw(char * msg) {
    enableNodalTransmit();
    int index = 0;
    while(msg[index]) {
        putUART2(msg[index]);
        ++index;
    }
    enableNodalReceive();
}


// REMEMBER TO ENABLE TRANSMIT
void putUART2(uint8_t data) {
    while(U2STAbits.UTXBF == 1);
    //putUART1(data);
    U2TXREG = data;
    //The following line may slow things down if we check after every byte, but it will
    //ensure we don't miss anything.  Can optimize later
    while(!U2STAbits.TRMT); //Wait for last transmission to complete
}

/*
 //Will send a string on the network
void SendNodalString(char * output){
    int len = strlen(output);
    int i;
    for(i = 0; i < len ; i++){
        putUART2(output[i]);
    }
}
 *
void putCharNodalCommSendFIFO(char data){
    *sendPointerU2 = data;
    sendPointerU2++;
    sendBufferLenU2++;
}

void putIntNodalCommSendFIFO(int data) {
    *sendPointerU2 = (data & 0xFF);
    sendPointerU2++;
    sendBufferLenU2++;

    *sendPointerU2 = ((data >> 8) & 0xFF);
    sendPointerU2++;
    sendBufferLenU2++;
}

void putFloatNodalCommSendFIFO(float * data){
    unsigned char *ptr;
    ptr = (unsigned char *)data;    
    ptr+=3;

    int i;
    for(i=3;i>=0;i--){
        *sendPointerU2 = *ptr;
        sendPointerU2++;
        sendBufferLenU2++;
        ptr--;
    }
}
*/

void onNodalSendOpportunity() {
    if (!cb_empty(&nodal_outgoing)) {
        NetworkMessage* nm = cb_popqueue(&nodal_outgoing);
        SendNetworkMessage(nm);
    }
}

void send_ping(uint8_t uid) {
    send_node_mb(uid, NODALMSG_PING, 0);
}

void connected_nodes(char* ignore) {
    if (getSetting(SETTINGS_NODE_TYPE) == 'A') {
        debug(DEBUG_NETWORK, "MASTER STATE: %d", getTPState());
        tpConnections();
        tpPingConnectedNodes();
    } else if (getSetting(SETTINGS_NODE_TYPE) == 'B') {
        if (getTPState() == SLAVE_DISCONNECTED) {
            debug(DEBUG_NETWORK, "NO CONNECTION");
        } else if (getTPState() == SLAVE_CONNECTED) {
            debug(DEBUG_NETWORK, "CONNECTED");
        }
        debug(DEBUG_NETWORK, "BUFFER LENGTH: %d", nodal_receive_next);
        /*int i;
        for (i=0; i < nodal_receive_next; ++i) {
            debug(DEBUG_NETWORK, "CHAR IN BUFFER %d", nodal_receive_buffer[i]);
        }*/

        MessageBuilder mb;
        make_mb(&mb, 20);
        mb_add_string(&mb, "HI FROM SLAVE");
        send_node_mb(getMasterUid(), 77, &mb);

    }

}


void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt( void ){
     int data = U2RXREG;

    if (nodal_unjamming_counter >= UNJAMMING_THRESHOLD) {
        if (data == UNJAMMING_END_SYMBOL) {
            nodal_unjamming_counter = 0;
            initNodalReceiveBuffer();
        }
    } else {
        if (data == UNJAMMING_SYMBOL) {
            ++nodal_unjamming_counter;
        } else {
            nodal_unjamming_counter = 0;
        }

        putNodalReceiveBuffer(data);

        if(nodal_receive_next >= nodal_receive_total_length){
            flushNodalReceiveBuffer(); //Converts the input buffer into a string
        }
    }



     U2STAbits.OERR = 0;
     IFS1bits.U2RXIF  = 0;   //U2RX flag set to 0
}

