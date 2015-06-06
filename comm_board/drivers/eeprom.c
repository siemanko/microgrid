#include <p33EP512GM710.h>
#include <spi.h>
#include "shared/utils.h"


//SPI EEPROM DEFINES
#define SS2      LATCbits.LATC10      // slave select pin

#define SYSCLK  40000000
// Macro to get array size in bytes
// note that array size can't be found after passing pointer to a function
#define LEN(x)  (sizeof(x) / sizeof(x[0]))
// 25LC512 EEPROM instructions
#define WREN    0x06    // write enable
#define RDSR    0x05    // read STATUS register
#define WRDI    0x04    // write disable
#define WRITE   0x02    // initialize start of write sequence
#define READ    0x03    // initialize start of read sequence
#define CE      0xc7    // erase all sectors of memory

//Following are written by Dan Strawser
void init_eeprom(){    
 
    ANSELCbits.ANSC10=0;        //PIN 71, SS2, set to digital
    ANSELBbits.ANSB8=0;         //pin 74, SCK2, to digital
    ANSELBbits.ANSB7=0;         //pin 72, SDO2, to digital
    
    TRISBbits.TRISB8 = 0;       //PIN 74, SCK2, RP40
    TRISCbits.TRISC13 = 1;       //PIN 73, SDI2, RPI61
    TRISBbits.TRISB7 = 0;       //PIN 72, SDO2, RP39
    TRISCbits.TRISC10 = 0;      //PIN 71, SS2, output

   // TRISGbits.TRISG6 = 0;
   // TRISGbits.TRISG7 = 1;
   // TRISGbits.TRISG8 = 0;

   // TRISAbits.TRISA12 = 0;

    RPOR3bits.RP40R = 9;        //PIN 74, SCK2, RP40
    RPINR22bits.SCK2R = 40;     //PIN 74, SCK2, RP40, (as input)
    RPOR2bits.RP39R = 8;        //PIN 72, SDO2, RP39
    RPINR22bits.SDI2R = 61;     //PIN 73, SDI2, RPI61 

    SS2 = 1;
    // SPI config
    // CKP (clock polarity control) = 0
    // CKE (clock edge control) = 1
    // 8-bit, Master Mode
    // Baud = 4MHz = Fpb/( 2*( SPI1BRG+1)) = 40MHz/( 2*( 4+1))
    SPI2CON1=0;
    SPI2CON2=0;
    SPI2STAT=0;

    SPI2CON1bits.DISSCK=0;
    SPI2CON1bits.DISSDO = 0;
    SPI2CON1bits.MODE16=0;
    SPI2CON1bits.CKE=1;
    SPI2CON1bits.CKP=0;
    SPI2CON1bits.SSEN = 0;
    SPI2CON1bits.SPRE = 0b10;
    SPI2CON1bits.PPRE=0;
    SPI2CON1bits.MSTEN=1;
    SPI2CON1bits.SMP=0;
    
    SPI2STATbits.SPIEN=1;
}


// SPI2_transfer() - write to and read from SPI2 buffer
int SPI2_transfer(int b){
   // SPI2BUF = b;                        // write to buffer for TX
    SPI2BUF = b;                        // write to buffer for TX
    //while(SPI2STATbits.SPITBF);
    while( !SPI2STATbits.SPIRBF);       // wait transfer complete
    return SPI2BUF;                     // read the received value
} // END SPI2_transfer()


// waitBusy() - checks if EEPROM is ready to be modified 
// and waits if not ready
int waitBusy()
{
    char status = 0;

    do{
        SS2 = 0;                         // Select EEPROM
        SPI2_transfer( RDSR);           // Read EEPROM status register       
        status = SPI2_transfer( 0);     // send dummy byte to receive incoming data
        SS2 = 1;                         // Release EEPROM
    }while( status & 0x01);             // write-in-progress while status<0> set to '1'

    return 0;
}


/* readEEPROM() - reads data from EEPROM
 *
 * Inputs:  address - address from 0-0xffff (64kB / 512kbits)
 *          loadArray - array to load EEPROM data to
 *          loadArray_size - number of bytes of EEPROM data to load into array */
int readEEPROM( unsigned short address, char* loadArray, unsigned short loadArray_size)
{
    unsigned short i;

    // Wait until EEPROM is not busy
    waitBusy();

    SS2 = 0;                         // Select EEPROM
    SPI2_transfer( READ);           // initiate read
    SPI2_transfer( address >> 8);   // address must be 16-bits but we're transferring it in two 8-bit sessions
    SPI2_transfer( address);
    // Request and store loadArray_size number of bytes into loadArray
    for( i=0 ; i<loadArray_size ; i++){

        loadArray[i] = SPI2_transfer( 0x00);    // send dummy byte to read 1 byte
    }
    SS2 = 1;                         // Release EEPROM
    return 0;
}

/* writeEEPROM() - write data to EEPROM
 *
 * Inputs:  address - address from 0-0xffff (64kB / 512kbits)
 *          storeArray - array of which contents are stored in EEPROM
 *          storeArray_size - number of bytes in array to store into EEPROM */
int writeEEPROM( unsigned short address, char* storeArray, unsigned short storeArray_size)
{
    unsigned short i;

    // Wait until EEPROM is not busy
    waitBusy();

    SS2 = 0;                 // Select EEPROM
    SPI2_transfer( WREN);   // Send WRITE_ENABLE command
    SS2 = 1;                 // Release EEPROM
    SS2 = 0;                 // Select EEPROM again after WREN cmd
    SPI2_transfer( WRITE);  // initiate write
    SPI2_transfer( address >> 8 );
    SPI2_transfer( address );
    // write 1 byte at a time from array
    // Big endian - MSB at lowest address (0 - first letter in string)
    for( i=0 ; i<storeArray_size; i++){
        // New write cycle must be initiated if writing across 128-byte pages
        if( (address+i)%128 == 0 && i!=0 ){
            SS2 = 1;                 // Release EEPROM
            waitBusy();
            SS2 = 0;                 // Select EEPROM
            SPI2_transfer( WREN);   // Send WRITE_ENABLE command
            SS2 = 1;                 // Release EEPROM
            SS2 = 0;                 // Select EEPROM again after WREN cmd
            SPI2_transfer( WRITE);  // initiate write
            SPI2_transfer( (address+i) >> 8 );
            SPI2_transfer( address+i );
        }
        SPI2_transfer( storeArray[i]);
    }
    SS2 = 1;                         // Release EEPROM

    return 0;
}

unsigned char eeprom_read_byte(unsigned short address){
    waitBusy();

    SS2 = 0;                         // Select EEPROM
    SPI2_transfer( READ);           // initiate read
    SPI2_transfer( address >> 8);   // address must be 16-bits but we're transferring it in two 8-bit sessions
    SPI2_transfer( address);
    
    unsigned char output = SPI2_transfer( 0x00);

    SS2 = 1;                         // Release EEPROM
    return output;
}

float eeprom_read_float(unsigned short address){

    float output;
    int i;

    waitBusy();

    SS2 = 0;                         // Select EEPROM
    SPI2_transfer( READ);           // initiate read
    SPI2_transfer( address >> 8);   // address must be 16-bits but we're transferring it in two 8-bit sessions
    SPI2_transfer( address);

     for(i=0 ; i< 4; i++){
          *((uint8_t*)&output + i) = SPI2_transfer(0x00);
     }

    SS2 = 1;                         // Release EEPROM
    return output;

}

int eeprom_read_int(unsigned short adress) {
    unsigned char bytes[2];
    int i;
    for (i=0; i<2; ++i) bytes[i] = eeprom_read_byte(adress+i);
    return bytes_to_int(bytes);
}

uint32_t eeprom_read_uint32(unsigned short adress) {
    unsigned char bytes[4];
    int i;
    for (i=0; i<4; ++i) bytes[i] = eeprom_read_byte(adress+i);
    return bytes_to_uint32(bytes);
}


int eeprom_write_byte(unsigned short address, unsigned char inputData){
    
    waitBusy();

    SS2 = 0;                 // Select EEPROM
    SPI2_transfer( WREN);   // Send WRITE_ENABLE command
    SS2 = 1;                 // Release EEPROM
    SS2 = 0;                 // Select EEPROM again after WREN cmd
    SPI2_transfer( WRITE);  // initiate write
    SPI2_transfer( address >> 8 );
    SPI2_transfer( address );
    SPI2_transfer(inputData);
    SS2 = 1;

    return 0;
}

int eeprom_write_float(unsigned short address, float input){

    int i;
    uint8_t buffer[4];
    float_to_bytes(input, buffer);

    waitBusy();

    SS2 = 0;                 // Select EEPROM
    SPI2_transfer( WREN);   // Send WRITE_ENABLE command
    SS2 = 1;                 // Release EEPROM
    SS2 = 0;                 // Select EEPROM again after WREN cmd
    SPI2_transfer( WRITE);  // initiate write
    SPI2_transfer( address >> 8 );
    SPI2_transfer( address );

    for( i=0 ; i<4; i++){
        // New write cycle must be initiated if writing across 128-byte pages
        if( (address+i)%128 == 0 && i!=0 ){
            SS2 = 1;                 // Release EEPROM
            waitBusy();
            SS2 = 0;                 // Select EEPROM
            SPI2_transfer( WREN);   // Send WRITE_ENABLE command
            SS2 = 1;                 // Release EEPROM
            SS2 = 0;                 // Select EEPROM again after WREN cmd
            SPI2_transfer( WRITE);  // initiate write
            SPI2_transfer( (address+i) >> 8 );
            SPI2_transfer( address+i );
        }
        SPI2_transfer(buffer[i]);
    }
    
    SS2 = 1;                         // Release EEPROM
    return 0;
}


void eeprom_write_int(unsigned short adress, int data) {
    unsigned char bytes[2];
    int_to_bytes(data, bytes);
    int i;
    for(i=0; i<2; ++i) eeprom_write_byte(adress+i, bytes[i]);
}
void eeprom_write_uint32(unsigned short adress, long data) {
    unsigned char bytes[4];
    uint32_to_bytes(data, bytes);
    int i;
    for(i=0; i<4; ++i) eeprom_write_byte(adress+i, bytes[i]);
}


/* eraseEEPROM() - erase entire contents of EEPROM
 *
 * See 25LC512 datasheet for ways to protect sections of data */
int eeprom_wipe()
{
    // Wait until EEPROM is not busy
    waitBusy();

    SS2 = 0;                 // Select EEPROM
    SPI2_transfer( WREN);   // Send WRITE_ENABLE command
    SS2 = 1;                 // Release EEPROM
    SS2 = 0;                 // Select EEPROM again after WREN cmd
    SPI2_transfer( CE);     // send CHIP_ERASE command
    SS2 = 1;                 // Release EEPROM

    return 0;
} // END eraseEEPROM()
