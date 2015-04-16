#include "ComputerInterface.h"
#include "ComputerCommunicationDriver.h"
#include "DateAndTime.h"
#include "DataLogger.h"
#include "lcd.h"
#include "stdlib.h"
#include "stdint.h"
#include "GridStateManagement.h"
#include "ComputerMessages.h"
#include "SolarForecastModule.h"
#include "Debugger.h"
#include "BatteryManagement.h"
#include "LibraryFunctions.h"




void receiveMessageFromComputer(char * input){
}

char * itoa2Digit(int i , char b[]){
    char const digit[] = "0123456789";
    char * str = b;
    str+=2;
    *str='\0';
    str--;
    *str=digit[i%10];
    str--;
    i/=10;
    *str=digit[i];
    return b;
}