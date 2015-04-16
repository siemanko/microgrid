/* 
 * File:   Debugger.h
 * Author: Dan
 *
 * Created on December 30, 2014, 10:43 PM
 */

#ifndef DEBUGGER_H
#define	DEBUGGER_H

void initDebugger();

void turnOnDebuggingLight();
void turnOffDebuggingLight();

void setDebuggerFlag();
void incrementDebuggerFlag();
int getDebuggerFlag();

#endif	/* DEBUGGER_H */

