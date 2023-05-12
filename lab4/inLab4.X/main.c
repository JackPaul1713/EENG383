//*****************************************************************
// Name:    Dr. Chris Coulston
// Date:    Fall 2020
// Lab:     04
// Purp:    Play Hot Cross Buns (B5, A5, G4) from ISR through terminal
//
// Assisted: The entire EENG 383 class
// Assisted by: Technical documents
//
// Academic Integrity Statement: I certify that, while others may have
// assisted me in brain storming, debugging and validating this program,
// the program itself is my own work. I understand that submitting code
// which is the work of other individuals is a violation of the course
// Academic Integrity Policy and may result in a zero credit for the
// assignment, course failure and a report to the Academic Dishonesty
// Board. I also understand that if I knowingly give my original work to
// another individual that it could also result in a zero credit for the
// assignment, course failure and a report to the Academic Dishonesty
// Board.
//*****************************************************************
#include "mcc_generated_files/mcc.h"
#pragma warning disable 520     // warning: (520) function "xyz" is never called  
#pragma warning disable 1498    // fputc.c:16:: warning: (1498) pointer (unknown)

#define SONG_LENGTH 3

uint8_t playNote = false;
uint8_t incNote = false;
uint8_t doSomethingBad = false;

void myTMR1ISR(void);

//*****************************************************************
//*****************************************************************

void main(void) {

    uint8_t i;
    char cmd;

    SYSTEM_Initialize();

    // Not necessary, but this delay allows the baud rate generator to 
    // stablize before printing the splash screen on reset. If you are going to
    // do this, then make sure to put delay BEFORE enabling TMR interrupt.
    TMR1_WriteTimer(0x0000);
    PIR1bits.TMR1IF = 0;
    while (PIR1bits.TMR1IF == 0);

    TMR1_SetInterruptHandler(myTMR1ISR);
    //INTERRUPT_GlobalInterruptEnable(); // ISR not working? - you probably 
    //INTERRUPT_PeripheralInterruptEnable(); // forgot to add these 2 lines

    printf("inLab 04\r\n");
    printf("Interrupt Music Box\r\n");
    printf("Dev'21\r\n");
    printf("Board wiring\r\n");
    printf("RB5 -> LPFin and install LPFout/AMPin jumper");
    printf("\r\n> "); // print a nice command prompt

    for (;;) {

        if (EUSART1_DataReady) { // wait for incoming data on USART
            cmd = EUSART1_Read();
            switch (cmd) { // and do what it tells you to do
                case '?':
                    printf("------------------------------\r\n");
                    printf("        TMR1 = 0x%04x\r\n", TMR1_ReadTimer());
                    printf("------------------------------\r\n");
                    printf("?: Help menu\r\n");
                    printf("o: k\r\n");
                    printf("Z: Reset processor\r\n");
                    printf("z: Clear the terminal\r\n");
                    printf("n/N: play/Stop playing a note\r\n");
                    printf("i: increment note in song.\r\n");
                    printf("b/B: start/stop printing in TMR1 ISR\r\n");
                    printf("t: time to print ""hello world""\r\n");
                    printf("------------------------------\r\n");
                    break;

                    //--------------------------------------------
                    // Start playing note
                    //--------------------------------------------                                          
                case 'n':
                    printf("Start playing note.\r\n");
                    playNote = true;
                    break;

                    //--------------------------------------------
                    // Stop playing note
                    //--------------------------------------------                                                              
                case 'N':
                    printf("Stop playing note.\r\n");
                    playNote = false;
                    break;

                    //--------------------------------------------
                    // Toggle the state of the note playing
                    //--------------------------------------------                                          
                case 'i':
                    printf("Playing next note.\r\n");
                    incNote = true;
                    break;

                    //--------------------------------------------
                    // Stop/Allow the ISR to print from inside the ISR
                    //--------------------------------------------                                          
                case 'b':
                case 'B':
                    if (cmd == 'b') doSomethingBad = true;
                    if (cmd == 'B') doSomethingBad = false;
                    break;

                    //--------------------------------------------
                    // Test time to print "hello world"
                    //--------------------------------------------    
                case 't':
                    INTERRUPT_GlobalInterruptDisable(); // ISR writes RC1
                    printf("Connect oscope to RC1.  Press any key when ready then press any key to resume.\r\n");

                    while (!EUSART1_DataReady); // wait for key press before proceeding
                    (void) EUSART1_Read(); // read character to clear flag
                    TEST_PIN_SetHigh();
                    printf("hello world\r\n");
                    TEST_PIN_SetLow();

                    while (!EUSART1_DataReady); // wait for key press before resuming
                    (void) EUSART1_Read(); // read character to clear flag                    
                    (void) EUSART1_Read(); // read character to clear flag 
                    INTERRUPT_GlobalInterruptEnable();

                    break;

                    //--------------------------------------------
                    // Simple handshake with the development board
                    //--------------------------------------------                                          
                case 'o':
                    printf("k.\r\n");
                    break;

                    //--------------------------------------------
                    // Reset the processor after clearing the terminal
                    //--------------------------------------------                      
                case 'Z':
                    for (i = 0; i < 40; i++) printf("\r\n");
                    RESET();
                    break;

                    //--------------------------------------------
                    // Clear the terminal
                    //--------------------------------------------                      
                case 'z':
                    for (i = 0; i < 40; i++) printf("\r\n");
                    break;

                    //--------------------------------------------
                    // If something unknown is hit, tell user
                    //--------------------------------------------
                default:
                    printf("Unknown key %c\r\n", cmd);
                    break;
            } // end switch
        } // end if        
    } // end infinite loop    
} // end main



//*****************************************************************
//*****************************************************************

void myTMR1ISR(void) {

    uint16_t halfPeriod[SONG_LENGTH] = {8112, 9088, 10208};
    static uint8_t noteIndex = 0;

    TEST_PIN_SetHigh();

    if (incNote == true) {
        noteIndex += 1;
        if (noteIndex >= SONG_LENGTH) noteIndex = 0;
        incNote = false; // Do it once
    }

    if (playNote == true) {
        TMR1_WriteTimer(0x10000 - halfPeriod[noteIndex]);
        SPEAKER_PIN_Toggle();
    } // end if 

    // Clear the TMR1 interrupt flag
    //PIR1bits.TMR1IF = 0;
    if (doSomethingBad == true) {
        printf("hello world\r\n");
    }

    TEST_PIN_SetLow();
}