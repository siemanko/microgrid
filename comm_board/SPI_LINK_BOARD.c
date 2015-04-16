#include "SPI_LINK_BOARD.h"
#include <pps.h>
#include "NodalCommunication.h"
#include <p33EP512GM710.h>
#include <spi.h>
#include "SPI.h"

#define SS_POWER        LATEbits.LATE1      // slave select pin

int SPI1_WriteFlag;     //To indicate that we are sending data and the interrupt should not ack an ack

void initSPI_LoadBoard(){

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
    SPI1CON1bits.SPRE = 11;
    SPI1CON1bits.PPRE=11;
    SPI1CON1bits.MSTEN=1;
    //SPI1CON1bits.SMP=0;
    SPI1CON1bits.SMP = 1;

    SPI1STATbits.SPIEN=1;

    IFS0bits.SPI1IF = 0;    //SPI1 interrupt flag
    IPC2bits.SPI1IP = 6;    //Interrupt priority for SPI1
    IEC0bits.SPI1IE = 0;    //Enable interrupt for SPI1 (KEEP DISABLED FOR NOW)

    SPI1BUF = 0;
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

int pollPowerBoard(){
    SPI1BUF = 6;
    while( !SPI1STATbits.SPIRBF);
    int buffer = SPI1BUF;
    return buffer;
}

int writeSPI1(int i)
{
    SPI1BUF = i;
    while(SPI1STATbits.SPITBF);
    while( !SPI1STATbits.SPIRBF); // wait for transfer complete
    int buffer = SPI1BUF;

    return buffer; // read the received value
}//writeSPI1


void __attribute__((__interrupt__,auto_psv)) _SPI1Interrupt(void){

    int buffer = SPI1BUF;

    //putUART2(buffer);

    //This code is for an ACK (if you'd like one)
    /*
    if(!SPI1_WriteFlag){    //Don't ack an ack
       SPI1BUF = 6;        //Acknowledge the receipt (ACK=6)
    } */

    SPI1STATbits.SPIROV = 0;
    IFS0bits.SPI1IF = 0;    //SPI1 interrupt flag
 }

