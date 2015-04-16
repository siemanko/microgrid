/*
 * File:   SPI_INTERFACE.h
 * Author: Dan
 *
 * Created on October 5, 2014, 10:59 PM
 */



#ifndef SPI_INTERFACE_H
#define	SPI_INTERFACE_H

void initSPI_EEPROM();
int SPI2_transfer(int b);
int waitBusy();
unsigned char readEEPROMByte(unsigned short address);
float readEEPROMFloat(unsigned short address);
int readEEPROM( unsigned short address, char* loadArray, unsigned short loadArray_size);
int writeEEPROMByte(unsigned short address, unsigned char inputData);
int writeEEPROMFloat(unsigned short address, void * inputData);
void writeEEPROMInt(unsigned short adress, int data);
void writeEEPROMLong(unsigned short adress, long data);
int readEEPROMInt(unsigned short adress);
int readEEPROMLong(unsigned short adress);

int writeEEPROM( unsigned short address, char* storeArray, unsigned short storeArray_size);
int eraseEEPROM();

#endif	/* SPI_INTERFACE_H */

