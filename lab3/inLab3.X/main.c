//*****************************************************************
// Name: Jack Martin, Jack Marley   
// Date: 1/26/2020
// Lab: 03
// Purp: torcher by MPLab (also play a song when upper button is pressed)
//
// Assisted: Dr. Chris Coulston (starter code)
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

//// compiler settings ////
#pragma warning disable 520     
#pragma warning disable 1498

//// constants ////
#define SCALE 30 // number of notes in the scale
#define SONG 30 // number of notes in the song

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

#define WHOLE 8 // whole note length (100ms)
#define HALF 4 // half note length (100ms)
#define QUARTER 1 // quarter note lenght (100ms)

//// function headers ////
void microSecondTimer(uint16_t us);
void milliSecondTimer(uint16_t ms);
void microSecondDelay(uint16_t us);
void milliSecondDelay(uint16_t ms);

//// main ////
void main(void) 
{
    // variables:
    uint8_t i = 0;
    uint16_t frequency = 0; // frequency (Hz)
    uint32_t period = 0; // period (us)
    uint16_t scale[SCALE] = {}; // note scale (frequency)
    uint8_t notes[SONG] = {A4, AS4, B4, C4, D4, DS4, E4, F4, FS4, G4, GS4, A5, AS5, B5, C5, CS5, D5, DS5, E5, F5, FS5, G5, GS5, A6, AS6, B6, C6, CS6, D6, DS6}; 
    uint8_t duration[SONG] = {QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER, QUARTER}; 
    // initialize:
    SYSTEM_Initialize();
    /* TMR0_Initialize();
    TMR1_Initialize(); */
    frequency = 440;
    for(i = 0; i < SCALE; i++) // get rid of this for simplicity 
    {
        frequency = frequency * 106 / 100; // calculate frequency (Hz), avoid decimals (eww)
        period = 1000000 / frequency; // calculate period (us)
        scale[i] = period / 2; // set scale to half the period
    }
    // play music:
    for(;;) 
    {
        // wait button press:
        if(TOP_BUTTON_GetValue() == 0) // if pressed
        {
            while(TOP_BUTTON_GetValue() == 0); // wait release

            // play:
            for(i = 0; i < SONG; i++) // for notes in song
            {
                // note:
                milliSecondTimer(duration[i]*100); // set timer for duration of note
                while(TMR0_HasOverflowOccured() == false) // for duration of note
                {
                    microSecondTimer(scale[notes[i]]); // set timer for half period
                    while(TMR1_HasOverflowOccured() == false); // wait duration of half period
                    SPEAKER_PIN_Toggle(); // toggle speaker
                }
                // pause:
                milliSecondTimer(100); // set timer for duration of pause
                while(TMR0_HasOverflowOccured() == false); // wait duration of pause
            }
        }
    } // infinite loop
}

//// timers ////
void milliSecondTimer(uint16_t ms)
{
    // variables:
    uint16_t counts = ms * 62.5; // 62.5 counts per milli second
    // set timer:
    TMR0_WriteTimer(0x10000 - counts); // set timer
    INTCONbits.TMR0IF = 0; // reset overflow
}
void microSecondTimer(uint16_t us)
{
    // variables:
    uint32_t counts = us * 16; // 16 counts per micro second
    // set timer:
    TMR1_WriteTimer(0x10000 - counts); // set timer
    PIR1bits.TMR1IF = 0; // reset overflow
}

//// delays ////
void milliSecondDelay(uint16_t ms) 
{
   // variables:
   uint16_t i;
   // delay:
   for (i = 0; i < ms; i++) microSecondDelay(1000);
}
void microSecondDelay(uint16_t us) 
{
   // variables:
   uint16_t i;
   // delay:
   for (i = 0; i < us; i++) 
   {
       asm("NOP"); // 1
       asm("NOP"); // 2
       asm("NOP"); // 3
       asm("NOP"); // 4
       asm("NOP"); // 5
       i = i;
   }  
}