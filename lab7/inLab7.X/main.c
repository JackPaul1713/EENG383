//--------------------------------------------------------------------
// Name:            Chris Coulston
// Date:            Fall 2020
// Purp:            inLab 7
//
// Assisted:        The entire class of EENG 383
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
#include "mcc_generated_files/mcc.h"
#pragma warning disable 520     // warning: (520) function "xyz" is never called  3
#pragma warning disable 1498    // fputc.c:16:: warning: (1498) pointer (unknown)

#define LED_ON      25 
#define LED_OFF     0
#define MAX_BUFFER_SIZE 32

uint8_t userEnter8bit(void);
void decodeIntoASCII(char msg[]);

void myTMR1ISR(void);
char transmitIRBuffer[MAX_BUFFER_SIZE];
uint8_t transmitStart = false;
uint8_t transmitBusy = false;
uint16_t bitPeriodInTMR1Counts = 13333; // 1200 Baud default

void myEUSART2ISR(void);
char recieveIRBuffer[MAX_BUFFER_SIZE];
uint8_t receiveBusy = false;
uint8_t receiveNewMessage = false;

char letter = '0';
uint8_t newCharacterToSend = false;

//----------------------------------------------
// Main "function"
//----------------------------------------------

void main(void) {

    uint8_t i;
    char cmd;

    SYSTEM_Initialize();
    EPWM2_LoadDutyValue(LED_OFF);

    // BEFORE enabling interrupts, otherwise that while loop becomes an
    // infinite loop.  Doing this to give EUSART1's baud rate generator time
    // to stabelize - this will make the splash screen looks better
    TMR1_WriteTimer(0x0000);
    PIR1bits.TMR1IF = 0;
    while (PIR1bits.TMR1IF == 0);

    // Since EUSART2 is double buffered, clear out any garbage with two reads from those buffers
    if (EUSART2_DataReady) (void) EUSART2_Read();
    if (EUSART2_DataReady) (void) EUSART2_Read();

    TMR1_SetInterruptHandler(myTMR1ISR);
    INTERRUPT_PeripheralInterruptEnable();
    INTERRUPT_GlobalInterruptEnable();


    printf("Lab 7\r\n");
    printf("Receive and decode an IR character\r\n");
    printf("Dev'21 Board wiring\r\n");
    printf("Install a jumper wire from RC0 to RB7 ONLY AFTER unplugging PICKit3\r\n");
    printf("Install a jumper over IR_TX header pins\r\n");
    printf("\r\n> "); // print a nice command prompt

    for (;;) {

        if (EUSART1_DataReady) { // wait for incoming data on USART
            cmd = EUSART1_Read();
            switch (cmd) { // and do what it tells you to do

                    //--------------------------------------------
                    // Reply with help menu
                    //--------------------------------------------
                case '?':
                    printf("-------------------------------------------------\r\n");
                    printf("2400 Baud\r\n");
                    printf("PR2: %d\r\n", PR2);
                    printf("-------------------------------------------------\r\n");
                    printf("?: help menu\r\n");
                    printf("o: k\r\n");
                    printf("Z: Reset processor\r\n");
                    printf("z: Clear the terminal\r\n");
                    printf("S: Send ""%c"" using IR transmitter\r\n", letter);
                    printf("R: use EUSART2 to decode character\r\n");
                    printf("r: reset EUSART2\r\n");
                    printf("p/P: decrease/increase PWM period by 1us\r\n");
                    printf("-------------------------------------------------\r\n");
                    break;

                    //--------------------------------------------
                    // Reply with "ok", used for PC to PIC test
                    //--------------------------------------------
                case 'o':
                    printf("o:	ok\r\n");
                    break;

                    //--------------------------------------------
                    // Reset the processor after clearing the terminal
                    //--------------------------------------------                      
                case 'Z':
                    for (i = 0; i < 40; i++) printf("\n");
                    RESET();
                    break;

                    //--------------------------------------------
                    // Clear the terminal
                    //--------------------------------------------                      
                case 'z':
                    for (i = 0; i < 40; i++) printf("\n");
                    break;

                    //--------------------------------------------
                    // Transmit letter through ECCP2 IR transmitter   
                    // Determine start-up and shut-down times  IR pulse -> IR decoder
                    // Determine fastest transmission rate to get good characters
                    //--------------------------------------------  
                case 'S':
                    // Preface character with a '0' bit
                    newCharacterToSend = true;
                    printf("just sent %c    %x\r\n", letter, letter);
                    letter += 1;
                    break;

                    //--------------------------------------------
                    // Read the a character from EUSART2 FIFO
                    //--------------------------------------------
                case 'R':
                    if (PIR3bits.RC2IF == 1)
                        printf("Just read in %c from EUSART2\r\n", EUSART2_Read());
                    else
                        printf("Nothing received from EUSART2\r\n");
                    break;

                    //--------------------------------------------
                    // reset EUSART2 in case it needs doing
                    //--------------------------------------------                
                case 'r':
                    RCSTA2bits.CREN = 0; // Try restarting EUSART2
                    RCSTA2bits.CREN = 1;
                    printf("Just reset EUSART2\r\n");
                    break;

                    //--------------------------------------------
                    // Change the period of the PWM and as a consequence the frequency
                    //--------------------------------------------                
                case 'p':
                case 'P':
                    if (cmd == 'p') PR2 -= 4;
                    if (cmd == 'P') PR2 += 4;
                    printf("Period = %d\r\n", PR2);
                    break;

                    //--------------------------------------------
                    // If something unknown is hit, tell user
                    //--------------------------------------------
                default:
                    printf("Unknown key %c\r\n", cmd);
                    break;
                    
            } // end switch            
        } // end if
    } // end while 
} // end main

typedef enum {
    TX_IDLE, TX_DATA_BITS
} myTXstates_t;
//----------------------------------------------
// My TMR1 ISR to handle incoming characters from IR decoder
//----------------------------------------------

void myTMR1ISR(void) {

    static myTXstates_t tmr1ISRstate = TX_IDLE;
    static uint8_t mask = 0b00000001;

    if (newCharacterToSend == true) {

        switch (tmr1ISRstate) {

                //---------------------------------
                // If you are here, it means that you've been inactive
                //---------------------------------
            case TX_IDLE:
                EPWM2_LoadDutyValue(LED_ON);
                tmr1ISRstate = TX_DATA_BITS;
                mask = 0b00000001;
                break;

                //---------------------------------
                // Send out 8 data bits
                //---------------------------------
            case TX_DATA_BITS:

                if (mask == 0) {
                    tmr1ISRstate = TX_IDLE;
                    EPWM2_LoadDutyValue(LED_OFF);
                    newCharacterToSend = false;
                } else {
                    if ((letter & mask) != 0) EPWM2_LoadDutyValue(LED_OFF);
                    else EPWM2_LoadDutyValue(LED_ON);
                }

                mask = mask << 1;
                break;

                //---------------------------------
                // How did you get here?
                //---------------------------------
            default:
                tmr1ISRstate = TX_IDLE;
                break;
        }
    }

    TMR1_WriteTimer(0x10000 - 6666); // 6666 = 2400 Baud
    PIR1bits.TMR1IF = 0;
}