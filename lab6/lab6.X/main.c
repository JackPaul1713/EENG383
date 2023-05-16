//--------------------------------------------------------------------
// Name:            Jack Martin, Jack Marley
// Date:            5/16/2023
// Purp:            lab 6, create uh like a super dope like IR message reciver
//
// Assisted:        I'm not entirly sure what this means
// Assisted by:     Microchips 18F26K22 Tech Docs 
//-
//- Academic Integrity Statement: I certify that, while others may have
//- assisted me in brain storming, debugging and validating this program,
//- the program itself is my own work. I understand that submitting code
//- which is the work of other individuals is a violation of the course
//- Academic Integrity Policy and may result in a zero credit for the
//- assignment, or course failure and a report to the Academic Dishonesty
//- Board. I also understand that if I knowingly give my original work to
//- another individual that it could also result in a zero credit for the
//- assignment, or course failure and a report to the Academic Dishonesty
//- Board.
//------------------------------------------------------------------------

//// includes ////
#include "mcc_generated_files/mcc.h"

//// compiler settings ////
#pragma warning disable 520     // warning: (520) function "xyz" is never called  3
#pragma warning disable 1498    // fputc.c:16:: warning: (1498) pointer (unknown)

//// defines ////
#define LED_ON      25 
#define LED_OFF     0

#define MAXBUFFERSIZE 32

//// global variables ////
typedef enum {RXDATA, RXCSUM} rxState_t;

uint8_t received = false; // pic18 has received a message (pic18 is always listening...)
uint8_t transmit = false; // pic18 is transmitting a message, lab 7?
char receiveBuffer[MAXBUFFERSIZE]; // received message
char transmitBuffer[MAXBUFFERSIZE]; // transmit message
uint8_t ri = 0; // receive buffer index
uint8_t ti = 0; // transmit buffer index 

uint8_t rsum = 0; // receive sum
uint8_t tsum = 0; // transmit sum
uint8_t rcsum = 0; // receive check sum

//// function headers ////
void rxEUSART2ISR(void);
void transmitCharacterOverIR(char letter, uint16_t baudRate);

//// main ////
void main(void) 
{
    // variables:
    uint8_t mask;
    uint16_t i;
    uint8_t baudRateSelected = 1;
    char cmd;
    char letter = '0';
    uint16_t bitPeriod[5] = {53333, 13333, 6666, 1666, 833};
    // initialize hardware:
    SYSTEM_Initialize();
    EPWM2_LoadDutyValue(LED_OFF);
    // stabilize baud rate generator:
    // delay so the baud rate generator is stable and prints the splash screen correctly
    TMR1_WriteTimer(0);
    PIR1bits.TMR1IF = 0;
    while (TMR1_HasOverflowOccured() == false);
    // clear out eusart2 buffers:
    // since EUSART2 is double buffered, clear out any garbage with two reads from those buffers
    if (EUSART2_DataReady) (void) EUSART2_Read();
    if (EUSART2_DataReady) (void) EUSART2_Read();
    // enable interrupts:
    PIE3bits.RC2IE = 0;
    EUSART2_SetRxInterruptHandler(rxEUSART2ISR);
    PIE3bits.RC2IE = 1;    
    INTERRUPT_PeripheralInterruptEnable();
    INTERRUPT_GlobalInterruptEnable();
    // print startup info:
    printf("inLab 6\r\n");
    printf("Receive and decode an IR packet\r\n");
    printf("Dev'21 Board wiring\r\n");
    printf("Install a jumper wire from RC0 to RB7 ONLY AFTER unplugging PICKit3\r\n");
    printf("Install a jumper over IR_TX header pins\r\n");
    printf("\r\n> "); // print a nice command prompt
    // actions:
    for (;;) 
    {
        if (EUSART1_DataReady) // check for incoming data on USART
        { 
            cmd = EUSART1_Read(); // read command from user
            printf("%c\n\r", cmd); // print selected command
            switch (cmd) // and do what it tells you to do
            {
                case '?': // print help menu
                    printf("-------------------------------------------------\r\n");
                    switch (baudRateSelected) 
                    {
                        case 0: printf("300 Baud\r\n");
                            break;
                        case 1: printf("1200 Baud\r\n");
                            break;
                        case 2: printf("2400 Baud\r\n");
                            break;
                        case 3: printf("9600 Baud\r\n");
                            break;
                        case 4: printf("19200 Baud\r\n");
                            break;
                        default: printf("1200 Baud\r\n");
                            break;
                    }
                    printf("-------------------------------------------------\r\n");
                    printf("?: help menu\r\n");
                    printf("o: k\r\n");
                    printf("Z: Reset processor\r\n");
                    printf("z: Clear the terminal\r\n");
                    printf("b: set the Baud rate of the sent characters\r\n");
                    printf("m: enter message\n\r");
                    printf("S: transmit message using IR \n\r");
                    printf("R: recive message using EUSART2 interrupts (from IR) \n\r");
                    printf("r: reset EUSART2\r\n");
                    printf("-------------------------------------------------\r\n");
                    break;
                case 'o': // reply with "ok" (used for PC to PIC test)
                    printf("o: ok\r\n");
                    break;                   
                case 'Z': // reset the processor after clearing the terminal
                    for (i = 0; i < 40; i++) printf("\n");
                    RESET();
                    break;                   
                case 'z': // clear the terminal
                    for (i = 0; i < 40; i++) printf("\n");
                    break;                  
                case 'b': // set the Baud rate (use MCC EUSART2 configuration register tab)
                    printf("Choose the index of the target baud rate\r\n");
                    printf("0: 300 baud\r\n");
                    printf("1: 1200 baud\r\n");
                    printf("2: 2400 baud\r\n");
                    printf("3: 9600 baud\r\n");
                    printf("4: 19200 baud\r\n");
                    while (EUSART1_DataReady);
                    baudRateSelected = EUSART1_Read() - '0';
                    switch (baudRateSelected) 
                    {
                        case 0: SPBRGH2 = 0xD0;
                            SPBRG2 = 0x54;
                            break;
                        case 1: SPBRGH2 = 0x34;
                            SPBRG2 = 0x14;
                            break;
                        case 2: SPBRGH2 = 0x1A;
                            SPBRG2 = 0x0A;
                            break;
                        case 3: SPBRGH2 = 0x06;
                            SPBRG2 = 0x82;
                            break;
                        case 4: SPBRGH2 = 0x03;
                            SPBRG2 = 0x40;
                            break;
                        default: SPBRGH2 = 0x34;
                            SPBRG2 = 0x14;
                            break;
                    }
                    printf("Baud rate assigned %02x:%02x\r\n", SPBRGH2, SPBRG2);
                    break;
                case 'm': // enter message, TODO
                    i = 0; // reset index
                    tsum = 0; // reset check sum
                    printf("Enter message, hit return when done.\n\r");
                    letter = EUSART1_Read(); // read letter
                    while(letter != '\r')
                    {
                        if(letter == '\b')
                        {
                            i--; // decrement index
                            tsum -= transmitBuffer[i]; // subtract previous letter from transmit sum
                        }
                        else
                        {
                            if(i > MAXBUFFERSIZE-2) break;
                            transmitBuffer[i] = letter; // add letter to transmit buffer
                            tsum += letter; // add letter to transmit sum
                            i++; // increment index
                        }
                        printf("%c", letter); // print letter
                        letter = EUSART1_Read(); // read new letter
                    }
                    transmitBuffer[i] = '\0';
                    i++;
                    transmitBuffer[i] = tsum;
                    printf("\n\rCreated\n\r  message: %s\n\r  checksum: %u\n\r", transmitBuffer, tsum); // print message and check sum
                    break;
                case 'S': // transmit message through IR
                    i = 0; // reset index
                    while(transmitBuffer[i] != '\0')
                    {
                        transmitCharacterOverIR(transmitBuffer[i], bitPeriod[baudRateSelected]); // transmit character
                        i++; // increment transmit index
                    }
                    transmitCharacterOverIR('\0', bitPeriod[baudRateSelected]); // transmit null character
                    transmitCharacterOverIR(tsum, bitPeriod[baudRateSelected]); // transmit check sum
                    printf("Tranmitted\n\r  message: %s\n\r  checksum: %u\n\r", transmitBuffer, tsum); // print message and check sum
                    break;
                case 'R': // receive message from ESUART2 (from IR)
                    if(received == true) // message received
                    {
                        printf("Received\n\r  message: %s\n\r  checksum: actual(%u), expected(%u)\n\r", receiveBuffer, rsum, rcsum); // print message, sum, and check sum
                        received = false; // set received to false
                    }
                    else printf("No message received\n\r"); // no message received
                    break;
                case 'r': // reset EUSART2 in case it needs doing
                    RCSTA2bits.CREN = 0; // Try restarting EUSART2
                    RCSTA2bits.CREN = 1;
                    printf("Just reset EUSART2\r\n");
                    break;
                default: // if something unknown is hit, tell user
                    printf("Unknown key %c\r\n", cmd);
                    break;
            }
            printf("\n\r> "); // print a nice command prompt
        }
    } // end while 
} // end main

//// interupts ////
void rxEUSART2ISR(void)
{
    printf("RxEUSART2ISR\n\r"); // DEBUG
    // variables:
    static uint8_t i = 0;
    uint8_t character;
    static rxState_t state = RXDATA;
    // receive:
    character = RCREG2; // read data
    if(received == false)
    {
        switch (state)
        {
            case RXDATA: // receive remaining characters
                printf("  RxData: %c\n\r", character); // DEBUG
                receiveBuffer[i] = character; // add character to received buffer
                if(i == 0) rsum = character; // initialize sum of characters
                else rsum += character; // add to sum of characters
                i++; // increment received buffer index
                if((character == '\0') || (i == MAXBUFFERSIZE-1)) state = RXCSUM; // change state
                break;
            case RXCSUM: // receive check sum
                printf("  RxCSum: %u\n\r", character); // DEBUG
                receiveBuffer[i] = character; // add check sum to received buffer
                rcsum = character; // set checksum
                i = 0; // reset received buffer index
                received = true; // end reception
                state = RXDATA; // change state
                break;
        }
    }
}

//// functions ////
void transmitCharacterOverIR(char letter, uint16_t baudRate)
{
    // transmit start bit:
    // need to preface character with a '0' bit
    EPWM2_LoadDutyValue(LED_ON);
    TMR1_WriteTimer(0x10000 - baudRate);
    PIR1bits.TMR1IF = 0;
    while (TMR1_HasOverflowOccured() == false);
    // transmit character bits:
    uint8_t mask = 0b00000001; // LSB first
    while (mask != 0) 
    {
        if ((letter & mask) != 0) EPWM2_LoadDutyValue(LED_OFF);
        else EPWM2_LoadDutyValue(LED_ON);
        mask = mask << 1;
        TMR1_WriteTimer(0x10000 - baudRate);
        PIR1bits.TMR1IF = 0;
        while (TMR1_HasOverflowOccured() == false);
    }
    // transmit stop bit:
    // need a stop bit to break up successive bytes
    EPWM2_LoadDutyValue(LED_OFF);
    TMR1_WriteTimer(0x10000 - baudRate);
    PIR1bits.TMR1IF = 0;
    while (TMR1_HasOverflowOccured() == false);
}

//// unused code ////
/* if(ri > MAXBUFFERSIZE)
// {
//     prinf("reception failed, max buffer size exceeded");
//     ri = 0; // reset received buffer index
//     state = RXBEG; // change state
// }
*/