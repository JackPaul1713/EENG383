//--------------------------------------------------------------------
// Name:            Chris Coulston
// Date:            Fall 2020
// Purp:            inLab11
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

#define ROM_LENGTH          16            // Add an extra array entry for interpolation
#define RATE                1875          // 

#define START_PHASE         3
#define END_PHASE           13

//----------------------------------------------
// Main "function"
//----------------------------------------------

void main(void) {

    uint8_t sin[ROM_LENGTH] = {128, 176, 218, 245, 255, 245, 218, 176, 128, 79, 37, 10, 0, 10, 37, 79};
    uint8_t i;
    char cmd;

    uint16_t a16, b16, c16, d16;
    uint32_t a32, b32, c32, d32;

    uint8_t phaseIncrement = 10;
    uint8_t phaseAccumulator = 0;
    uint8_t index;

    SYSTEM_Initialize();
    
    // Provide Baud rate generator time to stabilize before splash screen
    TMR0_WriteTimer(0x0000);
    INTCONbits.TMR0IF = 0;
    while (INTCONbits.TMR0IF == 0);

    //INTERRUPT_GlobalInterruptEnable();    
    //INTERRUPT_PeripheralInterruptEnable();

    printf("Lab 12\r\n");
    printf("DDS waveform generator\r\n");
    printf("Dev'21 Board wiring\r\n");
    printf("Connect jumper between RC2 and LPF_in.\r\n");
    printf("Connect jumper between LPF_out and AMP_in.\r\n");
    printf("Connect Channel 1 scope probe to RC2.\r\n");
    printf("Connect Channel 2 scope probe to LPF_out.\r\n");
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
                    printf("--	Fixed                       \r\n");
                    printf("--		Frequency:	%dHz        \r\n", phaseIncrement << 5);
                    printf("-------------------------------------------------\r\n");
                    printf("?:   help menu\r\n");
                    printf("o:   k\r\n");
                    printf("Z:   reset processor\r\n");
                    printf("z:   clear the terminal\r\n");
                    printf("s:   generate Sine wave\r\n");
                    printf("+/-: increase/decrease phase increment\r\n");
                    printf("w:   sWeep sine wave \r\n");
                    printf("m:   Math time\r\n");
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


                    //---------------------------------------------------------------			
                    // create a sine wave using simple DDS
                    //---------------------------------------------------------------                
                case 's':
                    phaseAccumulator = 0;
                    printf("Generating a %d Hz sine wave\r\n", phaseIncrement << 5);
                    while (EUSART1_DataReady == false) {
                        phaseAccumulator = phaseAccumulator + phaseIncrement;
                        index = phaseAccumulator >> 4;
                        EPWM1_LoadDutyValue(sin[index]);

                        TMR0_WriteTimer(0xFFFF - RATE); 
                        INTCONbits.TMR0IF = 0;
                        while (TMR0_HasOverflowOccured() == false);

                    }
                    (void) EUSART1_Read();
                    printf("Stopped generating sine wave\r\n");
                    EPWM1_LoadDutyValue(0x80);
                    break;

                    //---------------------------------------------------------------
                    // increase the phase increment to increase the frequency 
                    //---------------------------------------------------------------                
                case '+':
                case '-':
                    if (cmd == '+') phaseIncrement += 1;
                    else phaseIncrement -= 1;
                    printf("phaseIncrement = %d\r\n", phaseIncrement);
                    break;

                    //---------------------------------------------------------------
                    // start a frequency sweep 
                    //---------------------------------------------------------------  
                case 'w':
                    phaseAccumulator = 0;
                    printf("Press any key to exit.\r\n");
                    while (EUSART1_DataReady == false) {
                        for (phaseIncrement = START_PHASE; phaseIncrement < END_PHASE; phaseIncrement++) {

                            TMR1_WriteTimer(0x0000); 
                            PIR1bits.TMR1IF = 0;

                            while (TMR1_HasOverflowOccured() == false) {
                                phaseAccumulator = phaseAccumulator + phaseIncrement;
                                index = phaseAccumulator >> 4;
                                EPWM1_LoadDutyValue(sin[index]);

                                TMR0_WriteTimer(0xFFFF - RATE); 
                                INTCONbits.TMR0IF = 0;
                                while (TMR0_HasOverflowOccured() == false);
                            }
                        }
                    }
                    (void) EUSART1_Read();
                    printf("Stopped generation of sweeped sine wave\r\n");
                    EPWM1_LoadDutyValue(0x80);
                    break;

                    //--------------------------------------------
                    // Let's do some math!
                    //--------------------------------------------                
                case 'm':
                    a16 = 100;
                    b16 = 200;

                    printf("Performing: %u*500/%u:\r\n", a16, b16);

                    // The following lines of code took far too long to figure out
                    a32 = ((uint32_t) a16) * ((uint32_t) 500);
                    //printf("a32=0x%04x:%04x\r\n",((uint16_t) (a32>>16)), ((uint16_t) a32));      

                    b32 = (uint32_t) b16;
                    //printf("b32=0x%04x:%04x \r\n",((uint16_t) (b32>>16)), ((uint16_t) b32));                

                    c32 = a32 / b32;
                    //printf("c32=0x%04x:%04x\r\n",((uint16_t) (c32>>16)), ((uint16_t) c32));                

                    d16 = (uint16_t) c32;
                    //printf("Cast uint16_t to uint32_t, calculate with uint32_t, cast back to uint16_t: d16 paranoid = 0x%04x\r\n",(uint16_t) d16);
                    printf("d16 paranoid = 0x%04x = %u\r\n", d16, d16);

                    d16 = a16 * 500 / b16; // In danger of overflow during intermediate calculations
                    //printf("Performing all the computations in one expression: d16 casual = 0x%04x\r\n",d16);       
                    printf("d16 casual = 0x%04x = %u\r\n", d16, d16);

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
