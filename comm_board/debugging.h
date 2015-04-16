/* 
 * File:   debugging.h
 * Author: sidor
 *
 * Created on January 18, 2015, 4:35 PM
 */

#ifndef DEBUGGING_H
#define	DEBUGGING_H

#include <stdlib.h>
#include <stdarg.h>

#include "ComputerCommunicationDriver.h"
#include "ComputerMessages.h"
#include "MessageBuilder.h"
#include "settings.h"

#define DEBUG_MAIN          1
#define DEBUG_GRID_STATE    2
// whenever you see debug spam in code you can asume it's safe 
// to remove it.
#define DEBUG_SPAM          3
#define DEBUG_NETWORK       4
#define DEBUG_DATALOGGER    5
#define DEBUG_LPS           6
#define DEBUG_TOKENPASSING        7

// Call before debugging
void init_debug(void);

// Follows the syntax of printf (only %d and %f available)
void debug(unsigned char subsystem, const char* message, ...);

// Look at cmw_send and cmw_send_unsafe
void debug_unsafe(unsigned char subsystem, const char* message, ...);


// Send a string to subsystem.
void quick_debug(unsigned char subsystem, const char* message);

// Based on setings decides if debug should be printed.
int should_print_debug(int subsystem);

// Show/hide debug form a particular subsystem.
void set_debug(int subsystem, int should_work);



#endif	/* DEBUGGING_H */

