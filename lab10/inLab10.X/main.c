//--------------------------------------------------------------------
// Name:            Chris Coulston
// Date:            Fall 2020
// Purp:            inLab10
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

#define N                   32
#define RATE                3200           // 1:1 prescaler at (3200 counts) * (1us / 16 counts) = 200 us

//----------------------------------------------
// Main "function"
//----------------------------------------------

void main(void) {

    uint8_t sin[N] = {128, 153, 177, 199, 219, 234, 246, 254, 255, 254, 246, 234, 219, 199, 177, 153, 128, 103, 79, 57, 37, 22, 10, 2, 0, 2, 10, 22, 37, 57, 79, 103};
    uint8_t i;
    uint16_t index;
    char cmd;
    uint8_t volumeAdjust = 1;

    SYSTEM_Initialize();
    EPWM1_LoadDutyValue(0);
    // Provide Baud rate generator time to stabilize before splash screen
    TMR0_WriteTimer(0x0000);
    INTCONbits.TMR0IF = 0;
    while (INTCONbits.TMR0IF == 0);

    printf("inLab 10\r\n");
    printf("Analog waveform generator\r\n");
    printf("Dev'21\r\n");
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
                    printf("-------------------------------------------------\r\n");
                    printf("?: help menu\r\n");
                    printf("o: k\r\n");
                    printf("Z: Reset processor\r\n");
                    printf("z: Clear the terminal\r\n");
                    printf("+/-: increase/decrease volume\r\n");
                    printf("s: step through sine wave table with PIC 8-bit PWM\r\n");
                    printf("S: single step PWM duty cycle from 0%% to 100%%.\r\n");
                    printf("p: Generate a sine wave using PWM + LPF\r\n");
                    printf("-------------------------------------------------\r\n");
                    break;

                    //--------------------------------------------
                    // Reply with "k", used for PC to PIC test
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


                case '+':
                    volumeAdjust -= 1;
                    break;

                case '-':
                    volumeAdjust += 1;
                    break;

                    //---------------------------------------------------------------
                    // write to the PIC's PWM to generate sine wave
                    //---------------------------------------------------------------
                case 'S':
                    printf("Connect jumper over the RC2/LPF_in header pins.\r\n");
                    printf("Connect Channel 1 scope probe to RC2.\r\n");
                    printf("Connect Channel 2 scope probe to LPF_out.\r\n");

                    EPWM1_LoadDutyValue(0);
                    printf("Currently generating a 0%% duty cycle PWM waveform on RC2.\r\n");
                    printf("Press any key to change this to 100%.\r\n");
                    while (EUSART1_DataReady == false);
                    (void) EUSART1_Read();

                    EPWM1_LoadDutyValue(256);
                    printf("Currently generating a 100%% duty cycle PWM waveform on RC2.\r\n");
                    printf("Press any key to exit.\r\n");
                    while (EUSART1_DataReady == false);
                    (void) EUSART1_Read();

                    break;


                    //---------------------------------------------------------------
                    // write to the PIC's PWM to generate sine wave
                    //---------------------------------------------------------------
                case 's':
                    printf("Connect jumper over the RC2/LPF_in header pins.\r\n");
                    printf("Connect Channel 1 scope probe to RC2.\r\n");
                    printf("Connect Channel 2 scope probe to LPF_out.\r\n");
                    printf("Press ""x"" to exit.\r\n");
                    printf("Press any other key to step through sine table.\r\n\n");

                    index = 0;
                    do {
                        EPWM1_LoadDutyValue(sin[index]);
                        printf("Current LoadDutyValue = %d\r\n", sin[index]);
                        while (EUSART1_DataReady == false);
                        index = (index + 1)&0b00011111;
                    } while (EUSART1_Read() != 'x');

                    EPWM1_LoadDutyValue(0); // Park PWM at 0% duty cycle
                    printf("Exit PWM\r\n");
                    break;

                    //---------------------------------------------------------------
                    // write to the PIC's PWM to generate sine wave
                    //---------------------------------------------------------------
                case 'p':
                    printf("Connect jumper over the RC2/LPF_in header pins.\r\n");
                    printf("Connect Channel 1 scope probe to RC2.\r\n");
                    printf("Connect Channel 2 scope probe to LPF_out.\r\n");
                    printf("Press any key to exit.\r\n");

                    while (EUSART1_DataReady == false) {

                        TMR0_WriteTimer(TMR0_ReadTimer() + 0xFFFF - RATE);
                        INTCONbits.TMR0IF = 0;
                        while (TMR0_HasOverflowOccured() == false);

                        // EPWM1_LoadDutyValue(sin[index]);
                        EPWM1_LoadDutyValue(sin[index] >> volumeAdjust);
                        index = (index + 1)&0b00011111;

                    } // end while

                    EPWM1_LoadDutyValue(0); // Park PWM at 0% duty cycle
                    (void) EUSART1_Read(); // discard the junk character used to get us out of the loop                
                    printf("Exit PWM\r\n");
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