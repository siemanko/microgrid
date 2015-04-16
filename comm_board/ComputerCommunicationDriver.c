#include <p33EP512GM710.h>
#include <string.h>
#include <stdlib.h>
#include "ComputerCommunicationDriver.h"
#include "ComputerInterface.h"
#include "ComputerMessages.h"
#include "circular_buffer.h"
#include "debugging.h"
#include "lcd.h" // that should go away soon


#define FP 36850000
#define BAUDRATE 56700
#define BRGVAL ((FP/BAUDRATE)/16)-1
#define FIFO_SIZE_UART1 120 //Size of communication receive buffer

#define MAX_MESSAGE_LENGTH              120
#define MAX_QUEUED_OUTGOING             100
#define MAX_QUEUED_INCOMING             100

#define UNJAMMING_SYMBOL                69
#define UNJAMMING_END_SYMBOL            70
#define UNJAMMING_THRESHOLD             50



//Used for the input receive character buffer
static int communications_receive_next = 0;
static char communications_receive_buffer[FIFO_SIZE_UART1];

static unsigned int current_receive_total_length;

static int FIFOClearInterruptTimer;
static int currentFIFOClearInterruptTimer;

int communications_unjamming_counter = 0;

static CircularBuffer communications_outgoing;
static CircularBuffer communications_incoming;


// potentially we should do something to make sure this step is not
// taking too long (like limiting the number of messages processed
// in each step).
void computer_communication_step() {
    while(!cb_empty(&communications_incoming)) {
        char* message = (char*)cb_popqueue(&communications_incoming);
        // LCD_replace_line("M: ", LCD_TOP_ROW);
        // LCD_int(message[0]);
        handle_computer_message(message);
        free(message);
    }

    while(!cb_empty(&communications_outgoing)) {
        MessageBuilder* mb =
                (MessageBuilder*)cb_popqueue(&communications_outgoing);
        send_computer_mb_unsafe(mb);
        free(mb);
    }
}




static void initFIFOReceiveBufferU1(void){
    communications_receive_next = 0;
    current_receive_total_length = MAX_MESSAGE_LENGTH;
}


static void putComputerCommReceiveFIFO(int data_in){
    if (communications_receive_next < MAX_MESSAGE_LENGTH)
        communications_receive_buffer[communications_receive_next++] = data_in;

    if(communications_receive_next==2 && 0<= data_in && data_in <= MAX_MESSAGE_LENGTH)
        current_receive_total_length = data_in;
}

static void flushFIFOReceiveBufferU1(){
    char * inputStringU1 = (char *)malloc(current_receive_total_length*sizeof(char));

    int i;
    for (i=0; i<1000; ++i);

    for(i=0;i<current_receive_total_length;i++){
       inputStringU1[i]=communications_receive_buffer[i];
    }
    initFIFOReceiveBufferU1();

    cb_push(&communications_incoming, inputStringU1);
}



void initComputerCommunication(int UID) {
    initUART1(UID);
    initComputerMessageReceiveInterrupt(UID);

    make_cb(&communications_outgoing, MAX_QUEUED_OUTGOING);
    make_cb(&communications_incoming, MAX_QUEUED_INCOMING);

    register_misc_messages();
} 

void initUART1(int UID){

    U1STA = 0;
    U1MODE = 0;

    TRISAbits.TRISA8 = 1;
    TRISBbits.TRISB4 = 0;

    RPINR18bits.U1RXR = 24;
    RPOR1bits.RP36R = 1;

    U1MODEbits.STSEL = 0; // 1-Stop bit
    U1MODEbits.PDSEL = 0; // No Parity, 8-Data bits
    U1MODEbits.ABAUD = 0; // Auto-Baud disabled
    U1MODEbits.BRGH = 0; // Standard-Speed mode

    IFS0bits.U1RXIF = 0;
    IEC0bits.U1RXIE = 1;
    IPC2bits.U1RXIP = 1;

    U1STAbits.URXISEL = 0;

    U1BRG = BRGVAL;
    //U1BRG = 240;

    U1MODEbits.UARTEN = 1; // Enable UART
    U1STAbits.UTXEN = 1; // Enable UART TX
    
    initFIFOReceiveBufferU1();

    //Enables FIFO Reset
    //PR7 = FP/(3*256);
    //T7CONbits.TON = 1;
    //T7CONbits.TCKPS = 0b11;

    //IPC12bits.T7IP = 6;
   // IFS3bits.T7IF = 0;
   // IEC3bits.T7IE = 1;

    FIFOClearInterruptTimer = 10;
    currentFIFOClearInterruptTimer = 0;
}

void send_computer_mb(MessageBuilder* mb) {
    MessageBuilder* cpy = mb_copy(mb, 0);
    cb_push(&communications_outgoing, cpy);
}

void send_computer_mb_unsafe(MessageBuilder* mb) {
    int i;
    if (0 <= mb->next_char && mb->next_char <= MAX_MESSAGE_LENGTH) {
        for (i=0; i<mb->next_char; ++i) {
            putUART1((mb->message)[i]);
        }
        putUART1(0);
    } else {
        debug(DEBUG_MAIN, "DEBUG MESSAGE NOT BUILT CORRECTLY");
    }
    free(mb->message);
    // free(mb) would be wrong here.
}

void initComputerMessageReceiveInterrupt(int UID){

}

void putUART1(char data){
    while(U1STAbits.UTXBF == 1);
    U1TXREG = data;
}


void __attribute__((interrupt, no_auto_psv)) _U1RXInterrupt( void ){
    int data;

    if(U1STAbits.OERR == 1) U1STAbits.OERR = 0;
    
    data = U1RXREG;

    if (communications_unjamming_counter >= UNJAMMING_THRESHOLD) {
        if (data == UNJAMMING_END_SYMBOL) {
            communications_unjamming_counter = 0;
            initFIFOReceiveBufferU1();
        }
    } else {
        if (data == UNJAMMING_SYMBOL) {
            ++communications_unjamming_counter;
        } else {
            communications_unjamming_counter = 0;
        }

        putComputerCommReceiveFIFO(data);

        if(current_receive_total_length <= communications_receive_next) {
           flushFIFOReceiveBufferU1(); //Converts the input buffer into a string
        }
    }
    IFS0bits.U1RXIF = 0;
}

/*
void __attribute__((interrupt, no_auto_psv)) _T7Interrupt(){
    
    if(currentFIFOClearInterruptTimer==FIFOClearInterruptTimer){
        currentFIFOClearInterruptTimer=0;
        //initUART1(2);
        //putUART1(79);
    }else{
        currentFIFOClearInterruptTimer++;
    }
    IFS3bits.T7IF = 0;
} */
