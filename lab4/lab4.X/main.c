//*****************************************************************
// Name:    Jack Marley, Jack Martin
// Date:    Spring 2023
// Lab:     04
// Purp:    use interrupts
//
// Assisted: No one
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

//// includes ////
#include <math.h>

#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/tmr1.h"
#include "mcc_generated_files/mcc.h"

//// compiler ////
#pragma warning disable 520     // warning: (520) function "xyz" is never called  
#pragma warning disable 1498    // fputc.c:16:: warning: (1498) pointer (unknown)

//// defines ////
// scale:
#define SCALE 30 // number of notes in the scale
// notes:
#define A4 0
#define AS4 1
#define B4 2
#define C4 3
#define D4 4
#define DS4 5
#define E4 6
#define F4 7
#define FS4 8
#define G4 9
#define GS4 10
#define A5 11
#define AS5 12
#define B5 13
#define C5 14
#define CS5 15
#define D5 16
#define DS5 17
#define E5 18
#define F5 19
#define FS5 20
#define G5 21
#define GS5 22
#define A6 23
#define AS6 24
#define B6 25
#define C6 26
#define CS6 27
#define D6 28
#define DS6 29
// note lengths:
#define WHOLE 60000 // timer 0 ticks
#define HALF 30000 // timer 0 ticks
#define QUARTER 15000 // timer 0 ticks
#define PAUSE 20000 // timer 0 ticks
#define DELTA 5000 // timer 0 ticks
// songs:
#define WIN_SONG 0
#define TEST_SONG 1
#define LOSE_SONG 2
#define WIN_LENGTH 12
#define TEST_LENGTH 3
#define LOSE_LENGTH 4
#define MAX_LENGTH 12
// buttons:
#define BUTTON_PRESSED 0
#define BUTTON_UNPRESSED 1

//// data types ////
typedef enum{IDLE, PLAY_NOTE, PLAY_REST} tmr0State_t;

//// global variables ////
uint8_t noteIndex = 0;
uint8_t songSelection = 0;
uint8_t songLength = 0;

uint8_t playNote = false; // tells ISR1 to play a note
uint8_t playSong = false; // tells ISR0 to start playing song

uint16_t duration[3][MAX_LENGTH] = {{QUARTER, QUARTER, WHOLE, QUARTER, QUARTER, WHOLE, HALF, QUARTER, QUARTER, QUARTER, WHOLE, QUARTER}, // win song
                                   {HALF, HALF, HALF}, // test song
                                   {HALF, QUARTER, HALF, WHOLE}}; // lose song

//// function headers ////
void myTMR0ISR(void);
void myTMR1ISR(void);

//// main ////
void main(void) 
{
    // variables:
    uint8_t i;
    char cmd; // command
    uint16_t startTime = 0;
    uint16_t stopTime = 0;
    uint16_t overallTime = 0;
    int32_t mintime = 0;
    int32_t maxtime = 0;
    // initialize:
    SYSTEM_Initialize();
    // stablize:
    TMR0_WriteTimer(0x0000);
    TMR1_WriteTimer(0x0000); 
    INTCONbits.TMR0IF = 0;
    while(INTCONbits.TMR0IF == 0);
    // enable interups:
    TMR0_SetInterruptHandler(myTMR0ISR);
    TMR1_SetInterruptHandler(myTMR1ISR);
    INTERRUPT_GlobalInterruptEnable(); // ISR not working? - you probably forgot to add these 2 lines
    INTERRUPT_PeripheralInterruptEnable();
    // print startup info:
    printf("inLab 04\r\n");
    printf("Interrupt Music Box\r\n");
    printf("Dev'21\r\n");
    printf("Board wiring\r\n");
    printf("RB5 -> LPFin and install LPFout/AMPin jumper");
    printf("\r\n> "); // print a nice command prompt
    // actions:
    for(;;) 
    {
        if(EUSART1_DataReady) // wait for incoming data on USART (universal syncronous/asycronous reciver/transmitter)
        {
            // read input:
            cmd = EUSART1_Read();
            // preform action:
            switch (cmd) 
            {
                case '?':
                    printf("------------------------------\r\n");
                    printf("        TMR1 = 0x%04x\r\n", TMR1_ReadTimer());
                    printf("------------------------------\r\n");
                    printf("?: Help menu\r\n");
                    printf("o: k\r\n");
                    printf("Z: Reset processor\r\n");
                    printf("z: Clear the terminal\r\n");
                    printf("p: play song once\r\n");
                    printf("r: Rhythm practice\r\n");
                    printf("------------------------------\r\n");
                    break;                                                                               
                case 'o': // simple handshake with the development board
                    printf("k.\r\n");
                    break;                  
                case 'Z': // reset the processor after clearing the terminal
                    for (i = 0; i < 40; i++) printf("\r\n");
                    RESET();
                    break;                      
                case 'z': // clear the terminal
                    for (i = 0; i < 40; i++) printf("\r\n");
                    break;                                         
                case 'p': // play song
                    // print confirmation:
                    printf("Play song once.\r\n");
                    printf("Playing.\r\n");
                    // select and play test song:
                    songSelection = TEST_SONG;
                    songLength = TEST_LENGTH;
                    playSong = true;
                    break;
                case 'r': // rythem practice
                    // print title:
                    printf("Rhythm practice.\r\n");
                    // print instructions: 
                    printf("Listen to the beat pattern.\r\n");
                    // select and play test song:
                    songSelection = TEST_SONG;
                    songLength = TEST_LENGTH;
                    playSong = true;
                    while(playSong == true); // wait duration of test song
                    // print instructions:
                    printf("Use the upper button to reproduce this pattern.\r\n");
                    // record user rythem:
                    for (uint8_t i = 0; i < songLength; i++) 
                    {
                        // record button press time:
                        INTERRUPT_GlobalInterruptDisable();
                        INTERRUPT_PeripheralInterruptDisable();
                        while(TOP_BUTTON_GetValue() == BUTTON_UNPRESSED); // wait for button press
                        TMR0_WriteTimer(0x10000); // UNESSISARY
                        startTime = TMR0_ReadTimer();
                        while(TOP_BUTTON_GetValue() == BUTTON_PRESSED); // wait for button release 
                        stopTime = TMR0_ReadTimer();
                        INTERRUPT_GlobalInterruptEnable();
                        INTERRUPT_PeripheralInterruptEnable();
                        overallTime = stopTime - startTime; //amount of time spent on button presses
                        // check press time:
                        mintime = duration[songSelection][i] - DELTA;
                        maxtime = duration[songSelection][i] + DELTA;
                        //print("duration: %u\r\npress time: %u\r\nrange: %u, %u", duration[songSelection][i], overallTime, mintime, maxtime); // DEBUG
                        if((overallTime > mintime) && (overallTime < maxtime) && (i == songLength-1)) 
                        {
                            printf("You win.\r\n");
                            // select and play win song:
                            songSelection = WIN_SONG;
                            songLength = WIN_LENGTH;
                            playSong = true;
                            while(playSong == true);
                            break;
                        }
                        else if((overallTime < mintime) || (overallTime > maxtime))
                        {
                            printf("You lose.\r\n");
                            // select and play lose song:
                            songSelection = LOSE_SONG;
                            songLength = LOSE_LENGTH;
                            playSong = true;
                            while(playSong == true);
                            break;
                        }
                    }
                    break;
                default: // if something unknown is hit, tell user
                    printf("Unknown key %c\r\n", cmd);
                    break;
            } // end switch
        } // end if        
    } // end infinite loop    
} // end main

//// interrupts ////
void myTMR0ISR(void)
{
    // variables:
    static tmr0State_t tmr0State = IDLE;
    // change state:
    switch (tmr0State)
    {
        case IDLE:
            if (playSong == true) 
            {
                // printf("state: IDLE\r\n"); // DEBUG
                tmr0State = PLAY_NOTE;
                // TMR0_WriteTimer(0x10000 - 1000); // quickly reenter ISR0
            }
            break;
        case PLAY_NOTE:
            if (playSong == true) 
            {
                // printf("state: PLAY_NOTE, %d\r\n", duration[songSelection][noteIndex]); // DEBUG
                playNote = true;
                tmr0State = PLAY_REST; // change state
                TMR0_WriteTimer(0x10000 - duration[songSelection][noteIndex]);
            }
            else // UNESSISARY
            {
                tmr0State = IDLE; // change state
            }
            break;   
        case PLAY_REST:
            if((playSong == true) && (noteIndex < songLength-1))
            {
                // printf("state: PLAY_REST, %d\r\n", playNote); // DEBUG
                playNote = false;
                noteIndex++;
                tmr0State = PLAY_NOTE; // change state
                TMR0_WriteTimer(0x10000 - PAUSE);
            }
            else 
            {
                playNote = false; // stop playing note
                playSong = false; // stop playing song (more of don't play song)
                noteIndex = 0; // reset note index
                tmr0State = IDLE; // change state
                // TMR0_WriteTimer(0x10000); // UNESSISARY
            }
            break;
    }
    // reset:
    INTCONbits.TMR0IF = 0; // clear the interrupt flag
}
void myTMR1ISR(void)
{
    // variables:
    static uint16_t scale[SCALE] = {18192, 17168, 16192, 15296, 14448, 13632, 12864, 12144, 11472, 10816, 10208, 9632, 9088, 8592, 8112, 7648, 7232, 6816, 6432, 6080, 5728, 5408, 5104, 4816, 4560, 4288, 4048, 3824, 3616, 3408, 3216, 256}; // note scale (frequency)
    static uint8_t notes[3][MAX_LENGTH] = {{D6, FS4, A4, D6, FS4, D6, A4, C6, D6, FS4, A4, D6}, // win song
                                           {E4, DS6, GS5}, // test song
                                           {B4, A5, A6, G5}}; // lose song
    // play note:
    if(playNote == true)
    {
        TMR1_WriteTimer(0x10000 - scale[notes[songSelection][noteIndex]]);
        SPEAKER_PIN_Toggle();
    }
    // reset:
    PIR1bits.TMR1IF = 0; // clear the interrupt flag
}
