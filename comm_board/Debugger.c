#include "Debugger.h"
#include <p33EP512GM710.h>

static int debugger_flag;

void initDebugger(){
    debugger_flag=0;
}

void turnOnDebuggingLight(){

}

void turnOffDebuggingLight(){

}

void setDebuggerFlag(){
    debugger_flag=1;
}

void incrementDebuggerFlag(){
    debugger_flag++;
}

int getDebuggerFlag(){
    return debugger_flag;
}
