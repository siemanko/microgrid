#include "uart.h"

#include <p33EP512GM710.h>

#include "shared/utils.h"

#define FP 35007500L
#define BAUDRATE 57600
#define BRGVAL ((FP/BAUDRATE)/16)-1

#define BAUDRATE2 57600
#define BRGVAL2 ((FP/BAUDRATE)/16)-1

static void (*uart_callback[2])(uint8_t);

void init_uart(uart_device u) {
    if (u == UART_DEVICE1) {
        U1STA = 0;
        U1MODE = 0;

        TRISAbits.TRISA8 = 1;
        TRISBbits.TRISB4 = 0;

        RPINR18bits.U1RXR = 24;
        RPOR1bits.RP36R = 1;

        U1MODEbits.STSEL = 1; // 2-Stop bit
        U1MODEbits.PDSEL = 1; // Even Parity, 8-Data bits
        U1MODEbits.ABAUD = 1; // Auto-Baud disabled
        U1MODEbits.BRGH  = 0; // Standard-Speed mode
        
        IFS0bits.U1RXIF = 0;
        IEC0bits.U1RXIE = 1;
        IPC2bits.U1RXIP = 3;

        U1STAbits.URXISEL = 0; // 1 received bit enough to trigger 
                               // interrupt

        U1BRG = BRGVAL;

        U1MODEbits.UARTEN = 1; // Enable UART
        U1STAbits.UTXEN = 1;   // Enable UART TX
                               // must be called after enable UART.
    } else if (u == UART_DEVICE2) {
        U2STA = 0;
        U2MODE = 0;
        
        TRISCbits.TRISC7 = 0;   //DRIVE ENABLE AS OUTPUT, C7, PIN78
        LATCbits.LATC7 = 0;     //Turn enable as low (to begin)

        ANSELBbits.ANSB9 = 0;

        TRISBbits.TRISB9 = 1;   //PIN 76, RB9 as INPUT
        TRISCbits.TRISC6 = 0;   //PIN 77, RC6 as OUTPUT

        RPINR19bits.U2RXR = 41;  //RPI 41, PIN 76, as U2RX
        RPOR6bits.RP54R = 3; //RP54, PIN 77, aS U2TX

        U2MODEbits.STSEL = 0; // 1-Stop bit
        U2MODEbits.PDSEL = 0; // No Parity, 8-Data bits
        U2MODEbits.ABAUD = 0; // Auto-Baud disabled
        U2MODEbits.BRGH = 0; // Standard-Speed mode

        IFS1bits.U2RXIF  = 0;   //U2RX flag set to 0

        IEC1bits.U2RXIE = 1;    //U2RX interrupt enabled
        U2STAbits.URXISEL = 0;  // 1 received bit enough to trigger 
                                // interrupt

        IPC7bits.U2RXIP = 2;    //U2RX interrupt priority
        U2BRG = BRGVAL2;

        U2MODEbits.UARTEN = 1; // Enable UART
        U2STAbits.UTXEN = 1; // Enable UART TX
    }
}

void uart_put(uart_device u, uint8_t data) {
    if (u == UART_DEVICE1) {
        //while(U1STAbits.UTXBF == 1); // while transmit buffer is
                                     // still full.
        //U1TXREG = data;
        //while(!U1STAbits.TRMT);      // until transmit buffer is empty.
    } else if (u == UART_DEVICE2) {
        // TODO(szymon): understand LATC7.
        LATCbits.LATC7 = 1;
        while(U2STAbits.UTXBF == 1);
        U2TXREG = data;
        while(!U2STAbits.TRMT);
        LATCbits.LATC7 = 0;
    }
}

void uart_set_callback(uart_device u, void (*callback)(uint8_t)) {
    uart_callback[u] = callback;
}

void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt( void ){
    if(U1STAbits.OERR == 1) U1STAbits.OERR = 0;
    
    uint8_t data;
    while (U1STAbits.URXDA == 1){
        data = U1RXREG;
    
        if (uart_callback[UART_DEVICE1]) {
            uart_callback[UART_DEVICE1](data);
        }
    }
    U1STAbits.PERR = 0;
    U1STAbits.FERR = 0;
    U1STAbits.OERR = 0;

    IFS0bits.U1RXIF = 0;    
}

void __attribute__((interrupt, no_auto_psv)) _U2RXInterrupt( void ){
    int data;
    while (U2STAbits.URXDA == 1){
        data = U2RXREG;

        if (uart_callback[UART_DEVICE2]) {
            uart_callback[UART_DEVICE2](data);
        }
    }
    U2STAbits.PERR = 0;
    U2STAbits.FERR = 0;
    U2STAbits.OERR = 0;
    IFS1bits.U2RXIF  = 0;   //U2RX flag set to 0
}