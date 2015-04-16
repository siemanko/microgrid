/* 
 * File:   DateAndTime.h
 * Author: Dan
 *
 * Created on December 17, 2014, 12:52 AM
 */

#ifndef DATEANDTIME_H
#define	DATEANDTIME_H

void initDateAndTime(long sysFrequency,int debugMode,int timeDivisor);

int getSeconds();
int getMinutes();
int getHours();
int getDays();
long int getTimestamp();

void setSeconds(int seconds);
void setMinutes(int minutes);
void setHours(int hours);
void setDays(int days);

void dateAndTimeStep(void);

void __attribute__((__interrupt__,auto_psv)) _T1Interrupt(void);

#endif	/* DATEANDTIME_H */

