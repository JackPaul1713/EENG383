//--------------------------------------------------------------------
// Name:            Jack Martin, Jack Marley
// Date:            2/23/2023
// Purp:            lab 7 (all the things lab 7 does)
//
// Assisted:        
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

//// compiler ////
#pragma warning disable 520     // warning: (520) function "xyz" is never called  3
#pragma warning disable 1498    // fputc.c:16:: warning: (1498) pointer (unknown)

//// defines ////
#define LED_ON 25 
#define LED_OFF 0
#define MAXBUFFERSIZE 32

#define TXSRC transmitBuffer[0]
#define TXDST transmitBuffer[1]
#define RXSRC receiveBuffer[0]
#define RXDST receiveBuffer[1]

#define TXMESSAGE (transmitBuffer + 2)
#define RXMESSAGE (receiveBuffer + 2)

//// function headers ////
uint8_t read8bit(void);
void decodeIntoASCII(char msg[]);
// interupts:
void txTMR1ISR(void);
void rxEUSART2ISR(void);

//// global variables ////
char character = '0';
uint16_t baudRateCounts[5] = {53333, 13333, 6666, 1666, 833};
uint8_t baudRateIndex = 2;
typedef enum {TXSTARTBIT, TXDATABITS, TXSTOPBIT} txState_t;
typedef enum {RXDATA, RXCSUM} rxState_t;
// transmit:
char transmitBuffer[MAXBUFFERSIZE];
uint8_t ti = 0; // transmit buffer index
uint8_t tsum = 0; // transmit sum
uint8_t transmitting = false; // transmitting characters (newCharacterToSend)
// uint8_t transmitBusy = false;
// receive:
char receiveBuffer[MAXBUFFERSIZE];
uint8_t ri = 0; // receive buffer index
uint8_t rsum = 0; // receive sum
uint8_t rcsum = 0; // receive check sum
uint8_t received = false; // received a message (pic18 is always listening...)
// uint8_t receiveBusy = false;

//// main ////
void main(void) 
{
    // variables:
    uint16_t i;
    char cmd;
    char charater = '\0';
    // initialize:
    TXSRC = '0'; // set source address
    TXDST = '1'; // set destination address
    SYSTEM_Initialize();
    EPWM2_LoadDutyValue(LED_OFF);
    // stabilize baud rate generator:
    // delay so the baud rate generator is stable and prints the splash screen correctly *before enabeling interupts*
    TMR1_WriteTimer(0x0000);
    PIR1bits.TMR1IF = 0;
    while (PIR1bits.TMR1IF == 0);
    // clear eusart2 buffers:
    // since EUSART2 is double buffered, clear out any garbage with two reads from those buffers
    if (EUSART2_DataReady) (void) EUSART2_Read();
    if (EUSART2_DataReady) (void) EUSART2_Read();
    // enable interrupts:
    TMR1_SetInterruptHandler(txTMR1ISR);
    PIE3bits.RC2IE = 0;
    EUSART2_SetRxInterruptHandler(rxEUSART2ISR);
    PIE3bits.RC2IE = 1;
    INTERRUPT_PeripheralInterruptEnable();
    INTERRUPT_GlobalInterruptEnable();
    // print startup info:
    printf("Lab 7\r\n");
    printf("Receive and decode an IR character\r\n");
    printf("Dev'21 Board wiring\r\n");
    printf("Install a jumper wire from RC0 to RB7 ONLY AFTER unplugging PICKit3\r\n");
    printf("\n\r> "); // print a nice command prompt
    // actions:
    for (;;) // infinite loop
    {
        if (EUSART1_DataReady) // wait for data on eusart1
        { 
            // read command:
            cmd = EUSART1_Read(); // read command from user
            printf("%c\n\r", cmd); // print selected command
            // execute command:
            switch (cmd) 
            {
                case '?': // print help menu
                    printf("-------------------------------------------------\r\n");
                    printf("2400 Baud\r\n");
                    printf("PR2: %d\r\n", PR2);
                    printf("-------------------------------------------------\r\n");
                    printf("?: help menu\r\n");
                    printf("o: k\r\n");
                    printf("Z: Reset processor\r\n");
                    printf("z: Clear the terminal\r\n");
                    printf("b: set Baud rate\n\r");
                    printf("m: create a NULL terminated message with SRC and DEST prefix\n\r");
                    printf("s: set Source transmit identity\n\r");
                    printf("d: set Destination transmit target\n\r");
                    printf("S: Send message using TMR1 ISR\n\r");
                    printf("R: Receive message using EUSART2 via IR decoder\n\r");
                    printf("M: Monitor all IR traffic\n\r");
                    printf("x/X: decode tx/RX message\n\r");
                    printf("r: reset EUSART2")
                    printf("-------------------------------------------------\r\n");
                    break;
                case 'o': // print "ok", used for PC to PIC test
                    printf("o:	ok\r\n");
                    break;                
                case 'Z': // reset the processor after clearing the terminal
                    for (i = 0; i < 40; i++) printf("\n");
                    RESET();
                    break;                    
                case 'z': // clear the terminal
                    for (i = 0; i < 40; i++) printf("\n");
                    break;
                case 'b': // select baud rate
                    // print baud rate menu:
                    printf("Choose the index of the target baud rate\r\n");
                    printf("0: 300 baud\r\n");
                    printf("1: 1200 baud\r\n");
                    printf("2: 2400 baud\r\n");
                    printf("3: 9600 baud\r\n");
                    printf("4: 19200 baud\r\n");
                    // read selection:
                    while(EUSART1_DataReady); // huh?
                    baudRateIndex = EUSART1_Read() - '0';
                    // set baud rate registers:
                    switch (baudRateIndex) 
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
                case 'm': // enter transmit message
                    // setup:
                    i = 2; // reset transmit index, leave space for scr and dst
                    tsum = TXSRC + TXDST; // reset check sum
                    // print instructions:
                    printf("Enter message, hit return when done.\n\r");
                    // read characters:
                    character = EUSART1_Read(); // read first character
                    while(character != '\r')
                    {
                        if(i > MAXBUFFERSIZE-2) break;
                        transmitBuffer[i] = character; // add character to transmit buffer
                        tsum += character; // sum character to transmit sum
                        i++; // increment transmit index
                        printf("%c", character); // print character
                        character = EUSART1_Read(); // read new character
                    }
                    transmitBuffer[i] = '\0'; i++; // add null character to transmit buffer, increment transmit index
                    transmitBuffer[i] = tsum; // add transmit sum to transmit buffer
                    // print message:
                    printf("\n\rCreated\n\r  message: %s\n\r  checksum: %u\n\r  source address: %u\n\r  destintation address: %u\n\r", TXMESSAGE, tsum, TXSRC, TXDST); // print message, transmit sum, and transmit source and destination
                    break;
                case 's': // set source identity
                    // print instructions:
                    printf("Enter source address, hit return when done.\n\r");
                    // read and convert digits:
                    TXSRC = read8bit();
                    // print source address:
                    printf("\n\rSet\n\r  source address: %u\n\r", TXSRC);
                    break;
                case 'd': // set destination identity
                    // print instructions:
                    printf("Enter destination address, hit return when done.\n\r");
                    // read and convert digits:
                    TXDST = read8bit();
                    // print source address:
                    printf("\n\rSet\n\r  destination address: %u\n\r", TXDST);
                    break;
                case 'S': // send message
                    transmitting = true;
                    // while(transmitting == true);
                    printf("Transmitting\n\r  message: %s\n\r  checksum: %u\n\r  source address: %u\n\r  destintation address: %u\n\r", TXMESSAGE, tsum, TXSRC, TXDST); // print message, transmit sum, and transmit source and destination
                    break;
                case 'R': // print received message
                    if(received == true) // message received
                    {
                        printf("Received\n\r  message: %s\n\r  checksum: actual(%u), expected(%u)\n\r  source address: %u\n\r  destintation address: %u\n\r", RXMESSAGE, rsum, rcsum, RXSRC, RXDST); // print message, recieve sum, recieve check sum, and recieve source and destination
                        received = false; // set received to false
                    }
                    else printf("No message received\n\r"); // no message received
                    break;
                case 'M': // monitor:
                    character = RCREG1;
                    printf("SRC DST SUM CHECK MESSAGE\n\r");
                    // printf("RCREG1 enter: %u\n\r", RCREG1); // DEBUG
                    while(character == RCREG1)
                    {
                        if(received == true) // message received
                        {
                            printf("  %u %u %u %u    %s\n\r", RXSRC, RXDST, rsum, rcsum, RXMESSAGE); // print receive source and destination, receive sum, receive check sum, and receive message
                            received = false; // set received to false
                        }
                    }
                    // printf("RCREG1 exit: %u\n\r", RCREG1); // DEBUG
                    // (void) EUSART2_Read();
                    break;
                case 'x': // decode transmit buffer
                    i = 0;
                    while(transmitBuffer[i] != '\0')
                    {
                        printf("%u: %x %c\n\r", i, transmitBuffer[i], transmitBuffer[i]); // print character
                        i++; // increment index
                    }
                    printf("%u: %x %c\n\r", i, transmitBuffer[i], transmitBuffer[i]); i++; // print null character, increment index
                    printf("%u: %x %c\n\r", i, transmitBuffer[i], transmitBuffer[i]); // print check sum
                    break;
                case 'X': // decode receive buffer
                    i = 0;
                    while(receiveBuffer[i] != '\0')
                    {
                        printf("%u: %x %c\n\r", i, receiveBuffer[i], receiveBuffer[i]); // print character
                        i++; // increment index
                    }
                    printf("%u: %x %c\n\r", i, receiveBuffer[i], receiveBuffer[i]); i++; // print null character, increment index
                    printf("%u: %x %c\n\r", i, receiveBuffer[i], receiveBuffer[i]); // print check sum
                    break;
                case 'r': // reset EUSART2 in case it needs doing
                    RCSTA2bits.CREN = 0; // Try restarting EUSART2
                    RCSTA2bits.CREN = 1;
                    printf("Just reset EUSART2\r\n");
                    break;
                default:
                    printf("Unknown key %c\r\n", cmd);
                    break;
            }
            printf("\n\r> "); // print a nice command prompt
        }
    }
}

//// interrupts ////
// transmit:
void txTMR1ISR(void) 
{
    // printf("TxEUSART2ISR\n\r"); // DEBUG
    // variables:
    static uint8_t i = 0;
    static uint8_t mask = 0b00000001;
    static txState_t state = TXSTARTBIT;
    // transmit:
    if(transmitting == true) 
    {
        switch(state) 
        {
            case TXSTARTBIT:
                // printf("TxStartBit: %c\n\r", transmitBuffer[i]); // DEBUG
                mask = 0b00000001; // reset mask
                EPWM2_LoadDutyValue(LED_ON); // transmit start bit
                state = TXDATABITS; // change state
                break;
            case TXDATABITS: // transmit characters
                // printf("TxDataBits: %u\n\r", ((transmitBuffer[i] & mask) != 0)); // DEBUG
                if ((transmitBuffer[i] & mask) != 0) EPWM2_LoadDutyValue(LED_OFF); // transmit one
                else EPWM2_LoadDutyValue(LED_ON); // transmit zero
                mask = mask << 1; // shift mask
                if(mask == 0) state = TXSTOPBIT; // change state
                break;
            case TXSTOPBIT:
                // printf("TxStopBit\n\r", i); // DEBUG
                EPWM2_LoadDutyValue(LED_OFF); // transmit stop bit
                if((transmitBuffer[i-1] == '\0') && (i > 0)) // if transmission completed
                {
                    i = 0; // reset index
                    transmitting = false; // end transmission
                }
                else i++; // else increment transmit index
                state = TXSTARTBIT; // change state
                break;
        }
    }
    TMR1_WriteTimer(0x10000 - baudRateCounts[baudRateIndex]);
    PIR1bits.TMR1IF = 0;
}
// receive:
void rxEUSART2ISR(void)
{
    // printf("RxEUSART2ISR\n\r"); // DEBUG
    // variables:
    static uint8_t i = 0;
    uint8_t character = '\0';
    static rxState_t state = RXDATA;
    // receive:
    character = RCREG2; // read data
    if(received == false)
    {
        switch (state)
        {
            case RXDATA: // receive remaining characters
                printf("RxData: %c\n\r", character); // DEBUG
                // putchar('1'); // DEBUG
                receiveBuffer[i] = character; // add character to received buffer
                if(i == 0) rsum = character; // initialize sum of characters
                else rsum += character; // add to sum of characters
                i++; // increment received buffer index
                if((character == '\0') || (i == MAXBUFFERSIZE-1)) state = RXCSUM; // change state
                break;
            case RXCSUM: // receive check sum
                printf("RxCSum: %u\n\r", character); // DEBUG
                // putchar('0'); // DEBUG
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
uint8_t read8bit()
{
    // variables:
    uint8_t numb = 0;
    uint8_t maxValue = 255;
    uint8_t digit = 0;
    uint8_t character = 0;
    uint8_t i = 0;
    // read 8 bit number:
    while(character != '\r')
    {
        // read:
        character = EUSART1_Read(); // read character
        // delete:
        if(character == 127) 
        {
            printf("%c", character); // unprint last character
            if(numb > 0) numb /= 10; // unshift number
            if(i > 0) i--; // decrement index
            continue;
        }
        // calculate:
        digit = character - '0'; // convert character to digit
        if((digit > 9) || (numb * 10 + digit > maxValue)) continue; // ignore invalid digits
        if((numb == 0) && (digit == 0) && (i > 0)) continue; // ignore excess leading zeros
        if((numb == 0) && (i > 0)) printf("%c", 127); // remove leading zero
        printf("%c", character); // print character
        numb *= 10; // shift number
        numb += digit; // add digit to number
        i++; // increment index
    }
    // return:
    return(numb);
}

// uint32_t readInteger(uint32_t maxSize)
// {
//     // variables:
//     uint8_t i;
//     uint32_t numb = 0;
//     uint8_t digit = 0;
//     uint8_t character = 0;
//     // read 8 bit number:
//     for(;;)
//     {
//         character = EUSART1_Read(); // read character
//         if(character == '\r') break; // check
//         digit = character - '0'; // convert character to digit
//         if(digit > 9) {i--; continue;} // ignore invalid digits
//         if(numb * 10 > maxSize) {i--; continue;} // ignore too large numbers
//         printf("%c", character);
//         numb *= 10; // shift number
//         numb += digit; // add digit to number
//     }
//     // return:
//     return(numb);
// }
// void readString(uint8_t* string, uint16_t size)
// {
//     // variables:
//     uint8_t i = 0;
//     uint8_t sum = 0;
//     char character;
//     // read characters:
//     character = EUSART1_Read(); // read first character
//     while((character != '\r') && (i < size-2))
//     {
//         string[i] = character; // add character to string
//         sum += character; // sum character to sum
//         i++; // increment index
//         printf("%c", character); // print character
//         character = EUSART1_Read(); // read new character
//     }
//     string[i] = '\0'; i++; // add null character to string, increment index
//     string[i] = tsum; // add sum to string
// }
