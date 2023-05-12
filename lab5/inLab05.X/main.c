//--------------------------------------------------------------------
// Name:            Chris Coulston
// Date:            Fall 2020
// Purp:            inLab05
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
#include <inttypes.h>
#pragma warning disable 520     // warning: (520) function "xyz" is never called  
#pragma warning disable 1498    // fputc.c:16:: warning: (1498) pointer (unknown)

#define DUTY_INC    0x10

//----------------------------------------------
// Main "function"
//----------------------------------------------

void main(void) {

    uint8_t var8 = 0xAB;
    uint16_t var16 = 0xABCD;
    uint32_t var32 = 0xABCD0123;

    uint16_t duty = 127;
    uint8_t i;
    char cmd;

    SYSTEM_Initialize();
    
    // Not necessary, but this delay allows the baud rate generator to 
    // stablize before printing the splash screen on reset. If you are going to
    // do this, then make sure to put delay BEFORE enabling TMR interrupt.
    TMR0_WriteTimer(0x0000);
    INTCONbits.TMR0IF = 0;
    while(TMR0_HasOverflowOccured() == false);
    
    printf("inLab 05\r\n");
    printf("Color Cube\r\n");
    printf("Dev'21 board wiring\r\n");
    printf("RC2 <-> Red LED");
    printf("\r\n> "); // print a nice command prompt

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
                    printf("f: print using several format specifiers.\r\n");
                    printf("e: print several escaped characters.\r\n");
                    printf("t: Display current TMR2\r\n");
                    printf("T: Display current TMR0\r\n");
                    printf("p/P: decrease/increase PWM duty cycle\r\n");
                    printf("------------------------------\r\n");
                    break;

                    //--------------------------------------------
                    // print out variables using different format specifiers
                    //--------------------------------------------                     
                case 'f':
                    printf("%%c %c\r\n", var8);
                    printf("%%u %u\r\n", var8);
                    printf("%%d %d\r\n", var8);
                    printf("%%x %x\r\n", var8);
                    printf("%%04x %04x\r\n\r\n", var8);

                    printf("%%c %c\r\n", var16);
                    printf("%%u %u\r\n", var16);
                    printf("%%d %d\r\n", var16);
                    printf("%%x %x\r\n", var16);
                    printf("%%04x %04x\r\n\r\n", var16);

                    printf("%%c %c\r\n", var32);
                    printf("%%u %u\r\n", var32);
                    printf("%%d %d\r\n", var32);
                    printf("%%x %x\r\n", var32);

                    printf("%04x:", var32 >> 16);
                    printf("%04x\r\n", var32 & 0xFFFF);

                    printf("%08" PRIx32 "\r\n", var32);
                    break;


                    //--------------------------------------------
                    // Try using some escaped characters in printf
                    //--------------------------------------------                     
                case 'e':
                    printf("This is how you print a quote \"  \r\n");
                    printf("This is how you print a percent %% \r\n");
                    printf("This is how you print a backslash \\  \r\n");
                    printf("Line with just a new line \n");
                    printf("Line with just a carriage return\r");
                    printf("here??\r\n");
                    printf("This is a horizontal tab \t in line.\r\n");
                    printf("This is a backspace \bcharacter.\r\n");
                    break;

                    //--------------------------------------------
                    // Reply with "k", used for PC to PIC test
                    //--------------------------------------------    
                case 'o':
                    printf(" k\r\n");
                    break;

                    //--------------------------------------------
                    // Read the display current timer2 value
                    //--------------------------------------------                      
                case 't':
                    printf("TMR2 = %04X\r\n", TMR2_ReadTimer());
                    break;

                    //--------------------------------------------
                    // Read the display current timer0 value
                    //--------------------------------------------                      
                case 'T':
                    printf("TMR0 = %04X\r\n", TMR0_ReadTimer());
                    break;

                    //--------------------------------------------
                    // decrease the PWM duty cycle of CCP1 on RC2
                    //--------------------------------------------                      
                case 'p':
                    if (duty >= DUTY_INC) duty -= DUTY_INC;
                    EPWM1_LoadDutyValue(duty);
                    printf("The duty cycle is %d TMR2 counts high in a period of %d TMR2 counts.\r\n", duty, 0xFF);
                    break;

                    //--------------------------------------------
                    // increase the PWM duty cycle of CCP1 on RC2
                    //--------------------------------------------                                          
                case'P':
                    if (duty <= 0xFF - DUTY_INC) duty += DUTY_INC;
                    EPWM1_LoadDutyValue(duty);
                    printf("The duty cycle is %d TMR2 counts high in a period of %d TMR2 counts.\r\n", duty, 0xFF);
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
                    // If something unknown is hit, tell user
                    //--------------------------------------------
                default:
                    printf("Unknown key %c\r\n", cmd);
                    break;
            } // end switch
        } // end if
    } // end while 
} // end main