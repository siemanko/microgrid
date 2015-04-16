/*
 * File:   main.c
 * Author: Dan
 *
 * Created on November 24, 2014, 12:59 PM
 * Code for dsPIC33E, comm board
 */
#include <p33EP512GM710.h>
#include "LinkBoardInterface.h"
#include "LoadBoardInterface.h"
#include "SPI_LOAD_BOARD.h"
#include "SPI_EEPROM.h"
#include "NodalCommunicationDriver.h"
#include "NodalCommunicationInterface.h"
#include "ComputerCommunicationDriver.h"
#include "ComputerInterface.h"
#include "DateAndTime.h"
#include "DataLogger.h"
#include "Debugger.h"
#include "SPI_LOAD_BOARD.h"
#include "LibraryFunctions.h"
#include "delays.h"
#include "lcd.h"
#include "pininit.h"
#include "settings.h"
#include "debugging.h"
#include "NetworkCommConstants.h"
#include "Balance.h"
#include "UserInterface.h"
#include "LoadBoxGridManagement.h"
#include "LinkBoxGridManagement.h"

// Internal FRC Oscillator
_FOSCSEL(FNOSC_FRCPLL);  // FRC Oscillator
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_NONE);   // Clock Switching is enabled and Fail Safe Clock Monitor is disabled
						     // OSC2 Pin Function: OSC2 is Clock Output
                                                     // Primary Oscillator Mode: Disabled
_FWDT(FWDTEN_OFF);              // Watchdog Timer Enabled/disabled by user software
_FGS(GWRP_OFF); // General segment Write protect turned off
_FICD(ICS_PGD2 & JTAGEN_OFF) // Select Debug/Program Lines and JTAG OFF

#define SYS_FREQUENCY 36850000

#define DEBUG_MODE_OFF          0
#define DEBUG_MODE_ON           1
#define DEBUG_MODE_2_ON         2
#define DEBUG_MODE_3_ON         3

// Log data every seconds
#define DATA_LOGGING_FREQ_S     1

/* How big are different types?
 *
 * short 2
 * int 2
 * long 4
 * long long 8
 */

void initOscillator(){
  // setup internal clock for 70MHz/35MIPS
  // 7.37/2=3.685*43=158.455/2=79.2275
  CLKDIVbits.PLLPRE=0;        // PLLPRE (N2) 0=/2
  //PLLFBD=41;                  // pll multiplier (M) = +2
  //PLLFBD = 41;  //PUT BACK TO 41!

  PLLFBD = 38;  //PUT BACK TO 41
  //20 is good value
  CLKDIVbits.PLLPOST=0;       // PLLPOST (N1) 0=/2 
  RCONbits.SWDTEN = 0;
  while (!OSCCONbits.LOCK);
}

void init(void) {
    initOscillator();

    initRandomNumberGen();

    initSPI_EEPROM();

    // ONLY USE THIS CODE WHEN RUNNING THE BOARD FOR THE FIRST TIME.
    // resetSettings(1, 'B');

    if (!checkMagicNumber()) {
        slow_blink();
        setSetting(SETTINGS_EEPROM_MAGIC, 42);
        slow_blink();
        long i;
        for (i = 500000L; i>=0; --i);
        asm ("RESET");
    }


    unsigned char uid = getSetting(SETTINGS_UID);
    char type = getSetting(SETTINGS_NODE_TYPE);

    initComputerCommunication(uid);
    debug_unsafe(DEBUG_MAIN, "COMPUTER COMMUNICATION INIT");


    initNetworkCommunication();
    debug_unsafe(DEBUG_MAIN, "NETWORK INITIALIZED");

    // This serves as a proof that board booted
    debug_unsafe(DEBUG_MAIN, "THINGS ARE HAPPENING");

    initDateAndTime(SYS_FREQUENCY,DEBUG_MODE_OFF,40);

    debug_unsafe(DEBUG_MAIN, "DATETIME INITIALIZED");


    initDataLogger(DATA_LOGGING_FREQ_S);
    debug_unsafe(DEBUG_MAIN, "DATALOGGER INITIALIZED");


    initDebugger(); // THIS SHOULD EVENTUALLY GO AWAY
    init_debug();
    debug_unsafe(DEBUG_MAIN, "DEBUGGER INITIALIZED");


    pininit();
    debug_unsafe(DEBUG_MAIN, "PINS INITIALIZED");

    init_timer();
    debug_unsafe(DEBUG_MAIN, "TIMER INITIALIZED");



    if (getSetting((SETTINGS_NODE_TYPE)) == 'A') {
        initLinkBoardInterface();
        initLinkBoxGridManagment();

    } else if (getSetting(SETTINGS_NODE_TYPE) == 'B') {
        LCD_init();
        debug_unsafe(DEBUG_MAIN, "LCD INITIALIZED");
        initLoadBoardInterface();
        initBalance();
        initUserInterface();
        initLoadBoxGridManagement();
    }
    debug_unsafe(DEBUG_MAIN, "LOAD/LINK BOARD STUFF INITIALIZED");

    debug_unsafe(DEBUG_MAIN, "INIT COMPLETE");

    debug_unsafe(DEBUG_MAIN, "ZMIERZ PINGI");
}

long last_siema = 0;
uint16_t lps = 0;
void show_lps(void) {
    if (last_siema + 1 <= getTimestamp()) {
        last_siema = getTimestamp();
        debug(DEBUG_LPS, "LPS: %d", lps);

        lps = 0;
    }
}

int main() {
    blink_led();

    init();

    delay_ms(100);

    
    while(1) {
        // debug(DEBUG_MAIN, ""
        lps++;
        show_lps();
        computer_communication_step();
        dateAndTimeStep();
        dataLoggerStep();
        network_step();
        if (getSetting(SETTINGS_NODE_TYPE) == 'A') {
            LinkBoxGridManagmentStep();
        } else if (getSetting(SETTINGS_NODE_TYPE) == 'B') {
            LoadBoxGridManagementStep();
            balanceStep();
            userInterfaceStep();
        }
    }
    return 1;
}
   