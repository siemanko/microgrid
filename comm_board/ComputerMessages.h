/* 
 * File:   ComputerCommConstants.h
 * Author: Dan
 *
 * Created on January 2, 2015, 2:53 PM
 */

#include "ComputerCommunicationDriver.h"



#define PING_MESSAGE                        1
#define GET_TIME_FROM_PIC                   2
#define GET_MEMORY_DATA_FROM_PIC            3
#define GET_NODES_CONNECTED_TO_NETWORK      4
#define GET_STATE_OF_GRID                   5
#define PUT_SOLAR_FORECASTS                 6
#define INIT_SOLAR_FORECASTS                7
#define RESET_PIC                           9
#define GET_ENERGY_STATE                    12
#define SET_DEBUG_MODE                      13
#define SYNC_DATA                           14
#define SET_VALUE                           15
#define GET_VALUE                           16
#define GET_EEPROM                          17
#define RESET_DATALOGGER                    18
#define GET_DATALOGGER                      19

#define BYTES_PER_FLOAT                     4

#define MAX_COMPUTER_MESSAGE_HANDLERS 127


#define VALUE_HOURS 1
#define VALUE_MINUTES 2
#define VALUE_SECONDS 3
#define VALUE_BATTERY 4
#define VALUE_UID 5
#define VALUE_DAYS 6
#define VALUE_DEVICE_TYPE 7
#define VALUE_BALANCE 8

// IN GENERAL MESSAGES SHOULD BE REGISTERED IN SEPARATE FILE NOT HERE!
// BUT SOME MESSAGES DO NOT FIT ELSEWHERE.
// Those messages are sad because they have no home elsewhere :(
void register_computer_message_handler(int num, void (*handler)(char* msg));

void handle_computer_message(char* message);

void reset_pic(char* ignore);

void handle_ping(char* ignored);

void register_misc_messages();