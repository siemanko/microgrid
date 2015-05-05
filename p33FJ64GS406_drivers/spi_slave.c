#include "spi_slave.h"

#include <xc.h>
#include <spi.h>
#include <stdlib.h>

uint8_t (*handle_message)(uint8_t);


void init_spi_slave(){

    INTCON2 = 0;
    INTCON1 = 0;
    U1MODE = 0;

    TRISFbits.TRISF6 = 1;   //SCK2 as Input on G6
    TRISFbits.TRISF2 = 1;   //SDI2 as Input on G7
    TRISFbits.TRISF3 = 0;   //SDO2 as output on G8

    TRISBbits.TRISB14 = 1;   //CS as Input on G9

    SPI1CON1=0;
    SPI1STAT=0;
    SPI1STATbits.SPIROV = 0;
    SPI1STATbits.SPIEN = 0;
    SPI1CON2=0;

    SPI1CON1bits.DISSCK=0;
    SPI1CON1bits.DISSDO = 0;
    SPI1CON1bits.MODE16=0;
    SPI1CON1bits.SMP=0;
    SPI1CON1bits.CKE=0;
    SPI1CON1bits.CKP=0;
    SPI1CON1bits.SSEN = 1;
    SPI1CON1bits.PPRE=0;
    SPI1CON1bits.SPRE = 0;
    SPI1CON1bits.MSTEN=0;

    IFS0bits.SPI1IF = 0;
    IEC0bits.SPI1IE = 1;
    IPC2bits.SPI1IP = 2;    //Interrupt priority for SPI1

    SPI1STATbits.SPIEN=1;
}

void spi_slave_set_handle_message(uint8_t (*handler)(uint8_t)) {
    handle_message = handler;
}

static uint8_t request_message = 0;
static uint8_t reponse_message = 0;
int request_fulfilled;
static uint8_t send_state = 0;

void initate_send(uint8_t request, uint8_t response) {
    reponse_message = response;
    request_message = request;
    request_fulfilled = 0;
    send_state = 0;
}

void spi_slave_put_byte(uint8_t i) {
    uint8_t trahsbit = SPI1BUF;
    SPI1BUF = i;
}

long  long ctr = 0;


void __attribute__((__interrupt__,auto_psv)) _SPI1Interrupt(void){

    if (SPI1STATbits.SPIROV) {
        // error
        SPI1STATbits.SPIROV	= 0;			// clear overflow

        int trash = SPI1BUF;
        spi_slave_put_byte(55);
        IFS0bits.SPI1IF = 0;
    } else if (!SPI1STATbits.SPIRBF) {
        spi_slave_put_byte(41);
        IFS0bits.SPI1IF = 0;
    } else {
        // SPI1STATbits.SPIROV = 0;
        // SPI1STATbits.SPIROV = 0;

        uint8_t buffer = SPI1BUF;
        while (SPI1STATbits.SPITBF);
        if (buffer == LOAD_READ_AGAIN) {
            if (0 <= send_state && send_state < 20) {
               spi_slave_put_byte(69);
                ++send_state;
            } else if (20 <= send_state && send_state < 30 ) {
                if (reponse_message + 1 == 69) {
                    spi_slave_put_byte(70);
                } else {
                    spi_slave_put_byte(reponse_message+1);
                }
                ++send_state;
            } else if (30 <= send_state) {
                spi_slave_put_byte(reponse_message);
                request_fulfilled = 1;
            }
        } else {
            if (buffer == request_message && !request_fulfilled) {
                // If we received the same message but did not send reponse
                // yet, assume this is just retramsmission of the request
                // and do not recompute the answer. Out answers are
                // stateful, so recomputing is bad.
                send_state = 0;
            } else {
                uint8_t message = handle_message(buffer);
                initate_send(buffer, message);
                spi_slave_put_byte(69);
            }
        }

        IFS0bits.SPI1IF = 0;
    }
    if (ctr++>1000LL){
        init_spi_slave();
        ctr = 0;
    }
}
