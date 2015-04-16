/* 
 * File:   NodalCommunicationInterface.h
 * Author: Dan
 *
 * Created on December 20, 2014, 1:53 PM
 */

#ifndef NODALCOMMUNICATIONINTERFACE_H
#define	NODALCOMMUNICATIONINTERFACE_H


typedef enum msgType{
    float_message,
    int_message,
    float_int_message
} MessageType;

typedef struct msg{
    MessageType msgType;
    int targetUID;
    int originatingUID;
    int messageIdentifier;
    int intData;
    float floatData;
} NodalMessage;

void initNodalCommunicationInterface(int UID,char type,long sys_frequency, int debugMode);
void SendNodalMessage(NodalMessage * msg);
void ReconstructNodalMessages(char * byteStream);
void ProcessNodalMessage(char * msg);
void clearMessageReceivedTimer();
void putIntNodalCommSendFIFO(int data);

void get_connected_nodes_handler(char* ignored);

#endif	/* NODALCOMMUNICATIONINTERFACE_H */

