#include "SPI_POWER_BOARD.h"
#include <pps.h>
#include <string.h>
#include "NodalCommunicationDriver.h"
#include <p33EP512GM710.h>
#include <spi.h>
#include "SPI.h"


#define SS_POWER        LATEbits.LATE1      // slave select pin

int SPI1_WriteFlag;     //To indicate that we are sending data and the interrupt should not ack an ack

//From Stack overflow
static float raw2ieee(uint8_t raw[]){
    // either
    union {
        uint8_t bytes[4];
        float fp;
    } un;
    memcpy(un.bytes, raw, 4);
    return un.fp;
}

void initSPI_PowerBoard(){
    
   ANSELCbits.ANSC3=0;
   ANSELAbits.ANSA9=0;
   ANSELAbits.ANSA4=0;

   TRISCbits.TRISC3 = 0;    //RC3 as output for SCK1
   TRISAbits.TRISA9 = 1;    //RA9 as input for SDI
   TRISAbits.TRISA4 = 0;    //RA4 for output on SDO1
   TRISEbits.TRISE1 = 0;

    SS_POWER = 1;
   // SPI config
    // CKP (clock polarity control) = 0
    // CKE (clock edge control) = 1
    // 8-bit, Master Mode
    // Baud = 4MHz = Fpb/( 2*( SPI1BRG+1)) = 40MHz/( 2*( 4+1))
    SPI1CON1=0;
    SPI1CON2=0;
    SPI1STAT=0;

    SPI1CON1bits.DISSCK=0;
    SPI1CON1bits.DISSDO = 0;
    SPI1CON1bits.MODE16=0;
    SPI1CON1bits.CKE=0;
    SPI1CON1bits.CKP=0;
    SPI1CON1bits.SSEN = 0;
    SPI1CON1bits.SPRE = 0b10;
    SPI1CON1bits.PPRE=0;
    SPI1CON1bits.MSTEN=1;
    //SPI1CON1bits.SMP=0;
    SPI1CON1bits.SMP = 0;
    
    SPI1STATbits.SPIEN=1;

    SPI1_WriteFlag = 0;
}


void sendFloatPowerBoard(float data){
    SPI1_WriteFlag = 1;
    IEC0bits.SPI1IE = 0;    //DISABLE SPI1 INTERRUPT
    void *ptr = &data;
    int i;
    ptr+=3;

    for(i=3;i>=0;i--){
        //putUART2(*(char *)ptr);
       SS_POWER = 0;
       writeSPI1(*(char *)ptr);
       SS_POWER = 1;
       ptr--;
    }
    IEC0bits.SPI1IE = 1;    //Enable interrupt for SPI1
    SPI1_WriteFlag = 0;
}

void sendIntPowerBoard(int i){
    SPI1_WriteFlag = 1;
    IEC0bits.SPI1IE = 0;    //DISABLE SPI1 INTERRUPT
    SS_POWER = 0;
    writeSPI1(i);
    SS_POWER = 1;
    IEC0bits.SPI1IE = 1;    //ENABLE SPI1 INTERRUPT
    SPI1_WriteFlag = 0;
    
}

int pollPowerBoard(int input){
    SPI1BUF = input;
    while( !SPI1STATbits.SPIRBF);
    int buffer = SPI1BUF;
    return buffer;
}

float getFloatPowerBoard(){
    
    uint8_t dataIn[4];
    int i=0;

    for(i=0;i<4;i++){
        SPI1BUF = 0;
        while( !SPI1STATbits.SPIRBF);
        dataIn[i] = SPI1BUF;
        //YOU MIGHT NEED A DELAY HERE TO WAIT FOR THE POWER BOARD TO SHIFT
        //THE DATA INTO ITS SPI REGISTER!!!
    }

    return raw2ieee(dataIn);
}

int writeSPI1(int i)
{
    SPI1BUF = i;
    //while(SPI1STATbits.SPITBF);
    while( !SPI1STATbits.SPIRBF); // wait for transfer complete
    int buffer = SPI1BUF;

    return buffer; // read the received value
}//writeSPI1