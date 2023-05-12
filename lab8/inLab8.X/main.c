//--------------------------------------------------------------------
// Name:            Chris Coulston
// Date:            Fall 2020
// Purp:            inLab08
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
#include <pic18f25k22.h>

#include "mcc_generated_files/mcc.h"
#pragma warning disable 520     // warning: (520) function "xyz" is never called  3
#pragma warning disable 1498    // fputc.c:16:: warning: (1498) pointer (unknown)

#define     NUM_SAMPLES     16
uint8_t NEW_SAMPLE = false;

void INIT_PIC(void);
void myTMR0ISR(void);

//----------------------------------------------
// Main "function"
//----------------------------------------------

void main(void) {

    uint8_t i, adc_reading[NUM_SAMPLES];
    char cmd;

    SYSTEM_Initialize();

    // BEFORE enabling interrupts, otherwise that while loop becomes an
    // infinite loop.  Doing this to give EUSART1's baud rate generator time
    // to stablize - this will make the splash screen looks better
    TMR0_WriteTimer(0x0000);
    INTCONbits.TMR0IF = 0;
    while (INTCONbits.TMR0IF == 0);

    TMR0_SetInterruptHandler(myTMR0ISR);
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    printf("inLab 08\r\n");
    printf("Microphone experiments\r\n");
    printf("Dev'21\r\n");
    printf("> "); // print a nice command prompt

    for (;;) {

        if (EUSART1_DataReady) { // wait for incoming data on USART
            cmd = EUSART1_Read();
            switch (cmd) { // and do what it tells you to do

                case '?':
                    printf("------------------------------\r\n");
                    printf("?: Help menu\r\n");
                    printf("o: k\r\n");
                    printf("Z: Reset processor\r\n");
                    printf("z: Clear the terminal\r\n");
                    printf("s: gather %d samples from ADC\r\n", NUM_SAMPLES);
                    printf("0-9: switch sampled channel to ANx\r\n");
                    printf("------------------------------\r\n");
                    break;

                    //--------------------------------------------
                    // Reply with "k", used for PC to PIC test
                    //--------------------------------------------    
                case 'o':
                    printf(" k\r\n>");
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
                    for (i = 0; i < 40; i++) printf("\n>");
                    break;

                    //--------------------------------------------
                    // Continue to collect samples until the user
                    // presses a key on the keyboard
                    //--------------------------------------------                                          
                case's':
                    for (i = 0; i < NUM_SAMPLES; i++) {
                        while (NEW_SAMPLE == false);
                        NEW_SAMPLE = false;
                        adc_reading[i] = ADRESH;
                    } // end while

                    printf("The last %d ADC samples from the microphone are:\r\n", NUM_SAMPLES);
                    for (i = 0; i < NUM_SAMPLES; i++) // print-out samples
                        printf("%d ", adc_reading[i]);
                    printf("\r\n");
                    break;

                case '0':
                case '1':
                case '2':
                case '3':
                case '4':
                case '5':
                case '6':
                case '7':
                case '8':
                case '9':
                    ADC_SelectChannel(cmd - '0');
                    printf("The ADC is now sampling AN%c\r\n", cmd);
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

//-----------------------------------------------------------------------------
// Start an analog to digital conversion every 100uS.  Toggle RC1 so that users
// can check how fast conversions are being performed.
//-----------------------------------------------------------------------------

void myTMR0ISR(void) {

    TEST_PIN_SetHigh(); // Set high when every we start a new conversion
    ADCON0bits.GO_NOT_DONE = 1; // start a new conversion
    NEW_SAMPLE = true; // tell main that we have a new value
    TMR0_WriteTimer(0x10000 - (1600 - TMR0_ReadTimer()));
    TEST_PIN_SetLow(); // Monitor pulse width to determine how long we are in ISR

} // end myTMR0ISR