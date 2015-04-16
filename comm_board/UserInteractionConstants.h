/* 
 * File:   UserInteractionConstants.h
 * Author: Dan
 *
 * Created on January 10, 2015, 11:14 PM
 */

//These need to be redefined... obviously (on an airplane writing this and do not have access to data sheet)
#include <p33EP512GM710.h>

#define GRID_NON_CRITICAL_ANSEL_OFF   ANSELAbits.ANSA9
#define GRID_NON_CRITICAL_OUTPUT      TRISAbits.TRISA9
#define GRID_NON_CRITICAL_WRITE       LATAbits.LATA9

#define GRID_CRITICAL_OUTPUT          TRISAbits.TRISA10
#define GRID_CRITICAL_WRITE           LATAbits.LATA10


