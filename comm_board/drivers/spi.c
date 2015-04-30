#include "spi.h"

#include <p33EP512GM710.h>
#include <string.h>

#include "shared/utils.h"

#define SPI_SLAVE_SELECT        LATAbits.LATA12

#define SPI_REQUEST_DATA                           0
#define SPI_READ_AGAIN                             11



// TODO(szymon): find out why regular delay does
// not work here
static void delay(short cycles){
    int i;int j;int k;
    //int p=0;
    for(i = 0;i<cycles;i++){
        for(j=0;j<cycles;j++){
            for(k=0;k<100;k++){
           }
        }
    }
}

void init_spi(){
    // Make dem digital
    ANSELAbits.ANSA12 = 0;
    ANSELGbits.ANSG6 = 0;
    ANSELGbits.ANSG7 = 0;
    ANSELGbits.ANSG8 = 0;

    // Set input/output
    TRISAbits.TRISA12 = 0;
    TRISGbits.TRISG6 = 0;
    TRISGbits.TRISG7 = 1;
    TRISGbits.TRISG8 = 0;

    // set up clocks
    RPOR10bits.RP118R = 32; //PIN10, SCK3, OUTPUT
    RPINR29bits.SCK3R = 118; //PIN10, SCK3, INPUT
    RPOR11bits.RP120R = 31; //PIN 12, SDO3, OUTPUT
    RPINR29bits.SDI3R = 119; //PIN 11, SDI3, INPUT

    
    SPI_SLAVE_SELECT = 1;
   // SPI config
    // CKP (clock polarity control) = 0
    // CKE (clock edge control) = 1
    // 8-bit, Master Mode
    // Baud = 4MHz = Fpb/( 2*( SPI1BRG+1)) = 40MHz/( 2*( 4+1))
    SPI3CON1=0;
    SPI3CON2=0;
    SPI3STAT=0;

    SPI3STATbits.SPIEN=0;                  // disable spi

    SPI3CON1bits.DISSCK = 0;               // Disable internal spi clock
    SPI3CON1bits.DISSDO = 0;               // SDPx controller by microchip
    SPI3CON1bits.MODE16=0;                 // enable 8 bit mode
    SPI3CON1bits.CKE= 0;                   // read on low clock
    SPI3CON1bits.CKP = 0;                  // clock polarity
    SPI3CON1bits.SSEN = 0;                 // we are master not slave
    SPI3CON1bits.SPRE = 0b1;               // prescaler
    SPI3CON1bits.PPRE = 0b11;              // prescaler
    SPI3CON1bits.MSTEN=1;                  // enable master
    SPI3CON1bits.SMP = 0;                  // input sampled in the middle of
                                           // output

    SPI3STATbits.SPIEN=1;                  // enable spi
    //flushSPIBuffer();

}

int getFloatLoadBoard(float* result){
    uint8_t dataIn[4];
    int i;

    delay(10);
    for(i=0; i<4; ++i) {
    // for(i=0; i<4; ++i) {
        if(!spi_request_byte(SPI_REQUEST_DATA, dataIn + i)) {
            return 0;
        }
        delay(10);
    }

   *result = bytes_to_float(dataIn);
   return 1;
}

uint8_t sendLoadBoardByteRaw(uint8_t message) {
    uint8_t trashBit = SPI3BUF;
    // while(SPI3STATbits.SPITBF);
    SPI3BUF = message;
    while( !SPI3STATbits.SPIRBF);

    int i;
    for (i=0; i<1000L; ++i);
    uint8_t ret = SPI3BUF;
    // while(SPI3STATbits.SPIRBF);
    for (i=0; i<1000L; ++i);
    return ret;
}

int spi_request_byte(uint8_t message, uint8_t* result) {
        SPI_SLAVE_SELECT = 0;

        //debug_unsafe(DEBUG_MAIN, "Trying to send %d", message);
        uint8_t ret = 0;
        int ctr;
        const int MAX_LOOPS = 250;
        ret = sendLoadBoardByteRaw(message);
        //debug_unsafe(DEBUG_MAIN, "Send message %d, first reply %d", message, ret);
        //debug_unsafe(DEBUG_MAIN, "Send message %d, second reply %d", message, ret);

        ctr = 0;
        while(ret != 69 && ctr++ < MAX_LOOPS) ret = sendLoadBoardByteRaw(message);
        if (ctr >= MAX_LOOPS) return 0;

        //debug_unsafe(DEBUG_MAIN, "Send received 69");

        ctr = 0;
        while(ret == 69 && ctr++ < MAX_LOOPS) ret = sendLoadBoardByteRaw(SPI_READ_AGAIN);
        if (ctr >= MAX_LOOPS) return 0;

        uint8_t different_value = ret;
        // debug_unsafe(DEBUG_MAIN, "Send got different: %d", ret);

        ctr = 0;
        while (ret == different_value  && ctr++ < MAX_LOOPS) ret = sendLoadBoardByteRaw(SPI_READ_AGAIN);
        if (ctr >= MAX_LOOPS) return 0;

        // debug_unsafe(DEBUG_MAIN, "Send got actual value %d", ret);
        SPI_SLAVE_SELECT = 1;
        *result = ret;
        return 1;
}

int spi_command(uint8_t command){
    uint8_t response;
    int send_successful = spi_request_byte(command, &response);
    return send_successful && (response == 0);
}

int spi_request_float(uint8_t command, float* result) {
    int command_send = spi_command(command);
    int float_received = getFloatLoadBoard(result);
    return command_send && float_received;
}