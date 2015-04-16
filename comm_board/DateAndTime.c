#include "DateAndTime.h"
#include "ComputerCommunicationDriver.h"
#include <p33EP512GM710.h>
#include "BatteryManagement.h"
#include "settings.h"

#define TIMER_OVERFLOWS_CORRECT_TIME             3

static int DateAndTimeSeconds;
static int DateAndTimeMinutes;
static int DateAndTimeHours;
static int DateAndTimeDays;


static int savedDays;
static int savedHours;
static int savedMinutes;

static int DateAndTimeSecondFlag;

static unsigned long system_frequency;
static int timer_overflows;

static int debug_mode;

void initDateAndTime(long sysFrequency,int debugMode, int timeDivisor){
    system_frequency=sysFrequency;debug_mode = debugMode;

    DateAndTimeSecondFlag=0;

    if(!debug_mode){
        timer_overflows=TIMER_OVERFLOWS_CORRECT_TIME;
        PR1 = system_frequency/(3*256);
    }else{
        PR1 = 1200;//system_frequency/(3*256*timeDivisor);
        timer_overflows=1;
    }

    T1CONbits.TON = 1;
    T1CONbits.TCKPS = 0b11;

    IPC0bits.T1IP = 6;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    
    DateAndTimeSeconds = 0;
    DateAndTimeMinutes = 0;
    DateAndTimeHours = 0;
    DateAndTimeDays = 0;

    setDays(getSetting(SETTING_TIME_DAYS));
    savedDays = getDays();

    setHours(getSetting(SETTING_TIME_HOURS));
    savedHours = getHours();

    setMinutes(getSetting(SETTING_TIME_MINUTES));
    savedMinutes = getMinutes();
}

static void updateAbsoluteTime(){

    DateAndTimeSeconds++;    

    if(DateAndTimeSeconds % 60 == 0){

        DateAndTimeSeconds=0;
        DateAndTimeMinutes++;

        if(DateAndTimeMinutes % 60 ==0){

            DateAndTimeMinutes=0;
            DateAndTimeHours++;

            if(DateAndTimeHours % 24 == 0){
                
                DateAndTimeHours=0;
                DateAndTimeDays++;

            } //End Hour Update

        }//End Minute Update

    } //End Second Update
}

int getSeconds(){
    return DateAndTimeSeconds;
}

int getMinutes(){
    return DateAndTimeMinutes;
}

int getHours(){
    return DateAndTimeHours;
}

int getDays() {
    return DateAndTimeDays;
}

long int getTimestamp() {
    return (long)getSeconds() +
           (long)getMinutes()*60 +
           (long)getHours()*60*60;
           (long)getDays()*60*60*24;
}

void setSeconds(int seconds){
    DateAndTimeSeconds=seconds;
}

void setMinutes(int minutes){
    DateAndTimeMinutes=minutes;
}

void setHours(int hours){
    DateAndTimeHours = hours;
}

void setDays(int days) {
    DateAndTimeDays=days;
}

void dateAndTimeStep(void) {
    if (savedDays != getDays()) {
        setSetting(SETTING_TIME_DAYS, getDays());
        savedDays = getDays();
    }
    if (savedHours != getHours()) {
        setSetting(SETTING_TIME_HOURS, getHours());
        savedHours = getHours();
    }
    if (savedMinutes != getMinutes()) {
        setSetting(SETTING_TIME_MINUTES, getMinutes());
        savedMinutes = getMinutes();
    }
}



void __attribute__((__interrupt__,auto_psv)) _T1Interrupt(void){
    DateAndTimeSecondFlag++;
    if(DateAndTimeSecondFlag==timer_overflows){
        DateAndTimeSecondFlag=0;
         updateAbsoluteTime();
    }

    IFS0bits.T1IF = 0;    
}












