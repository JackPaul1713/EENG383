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
//// includes ////
#include "mcc_generated_files/mcc.h"
#include <inttypes.h>

//// compiler ////
#pragma warning disable 520     // warning: (520) function "xyz" is never called  
#pragma warning disable 1498    // fputc.c:16:: warning: (1498) pointer (unknown)

//// defines ////
#define DUTY_INC 0x10

#define OFF 0xFF
#define ON 0x00

#define RED 0 
#define GREEN 1
#define BLUE 2

#define START 0x00
#define END 0xFF

#define HOLD 0x00
#define INCREASE 0xFF
#define DECREASE 0x01

#define TOURNUMBER 6

//// global variables ////
uint16_t period = 1000; // @1:16 0.001 s
uint8_t tour = false;
uint8_t rgbDuty[3] = {OFF, OFF, OFF};
uint8_t rgbDirection[3] = {HOLD, HOLD, HOLD};
uint8_t edgeLocation = START;
uint8_t tourDirectionIndex = 0;

#if TOURNUMBER == 6
    uint8_t rgbTourStart[3] = {OFF, OFF, ON};
    uint8_t tourDirections[6][3] = {{HOLD, INCREASE, HOLD}, {HOLD, HOLD, DECREASE}, {INCREASE, HOLD, HOLD}, {HOLD, DECREASE, HOLD}, {HOLD, HOLD, INCREASE}, {DECREASE, HOLD, HOLD}};
#elif TOURNUMBER == 3
    uint8_t rgbTourStart[3] = {ON, OFF, ON};
    uint8_t tourDirections[3][3] = {{DECREASE, INCREASE, HOLD}, {INCREASE, HOLD, DECREASE}, {HOLD, DECREASE, INCREASE}};
#endif

//// function headers ////
void TMR0ISR(void);

//// main ////
void main(void) 
{
    // variables:
    uint16_t duty = 127;
    uint8_t i;
    char cmd;
    // initialize:
    SYSTEM_Initialize();
    // stablize baud:
    TMR0_WriteTimer(0x0000);
    INTCONbits.TMR0IF = 0;
    while(INTCONbits.TMR0IF == 0); // wait for timer 0 to baud rate generator to stablize
    // start PWMS:
    EPWM1_LoadDutyValue(rgbDuty[RED]);
    EPWM2_LoadDutyValue(rgbDuty[GREEN]);
    EPWM3_LoadDutyValue(rgbDuty[BLUE]);
    // enable interupts:
    TMR0_SetInterruptHandler(TMR0ISR);
    INTERRUPT_GlobalInterruptEnable(); // ISR not working? - you probably forgot to add these 2 lines
    INTERRUPT_PeripheralInterruptEnable();
    // print startup info:
    printf("inLab 05\r\n");
    printf("Color Cube\r\n");
    printf("Dev'21 board wiring\r\n");
    printf("RC2 <-> Red LED");
    printf("\r\n> "); // print a nice command prompt
    // lab4:
    for(;;) 
    {
        if (EUSART1_DataReady) // wait for selection
        { 
            // read selection:
            cmd = EUSART1_Read();
            // actions:
            switch (cmd)
            { 
                case '?':
                    printf("------------------------------\r\n");
                    printf("?: Help menu\r\n");
                    printf("Z: Reset processor\r\n");
                    printf("z: Clear the terminal\r\n");
                    printf("R/r: increase/decrease Red intensity\r\n");
                    printf("G/g: increase/decrease Green intensity\r\n");
                    printf("B/b: increase/decrease Blue intensity\r\n");
                    printf("C/c: start/stop color cycle\r\n");
                    printf("a: All LEDs off\r\n");
                    printf("+/-: increase/decrease the color tour speed\r\n");
                    printf("------------------------------\r\n");
                    break;                         
                case 'Z': // reset the processor after clearing the terminal
                    for (i = 0; i < 40; i++) printf("\n");
                    RESET();
                    break;                 
                case 'z': // clear the terminal
                    for (i = 0; i < 40; i++) printf("\n");
                    break;
                case 'R': // add red
                    if(rgbDuty[RED] > ON) rgbDuty[RED]--;
                    break;
                case 'G': // add green
                    if(rgbDuty[GREEN] > ON) rgbDuty[GREEN]--;
                    break;
                case 'B': // add blue
                    if(rgbDuty[BLUE] > ON) rgbDuty[BLUE]--;
                    break;
                case 'r': // subtract red
                    if(rgbDuty[RED] < OFF) rgbDuty[RED]++;
                    break;
                case 'g': // subtract green 
                    if(rgbDuty[GREEN] < OFF) rgbDuty[GREEN]++;
                    break;
                case 'b': // subtract blue
                    if(rgbDuty[BLUE] < OFF) rgbDuty[BLUE]++;
                    break;
                case 'C': // start color tour
                    // set start duty cycle values:
                    rgbDuty[RED] = rgbTourStart[RED];
                    rgbDuty[GREEN] = rgbTourStart[GREEN];
                    rgbDuty[BLUE] = rgbTourStart[BLUE];
                    // set start tour directions:
                    rgbDirection[RED] = tourDirections[tourDirectionIndex][RED];
                    rgbDirection[GREEN] = tourDirections[tourDirectionIndex][GREEN];
                    rgbDirection[BLUE] = tourDirections[tourDirectionIndex][BLUE];
                    tourDirectionIndex = 0;
                    // set start edge location:
                    edgeLocation = START;
                    // start tour:
                    tour = true;
                    break;
                case 'c': // stop color tour
                    tour = false;
                    break;
                case '+': // increase interupt speed
                    period += 200; // 0.0002 s
                    printf("%u\n\r", period); // DEBUG
                    break;
                case '-': // decrease interupt speed
                    period -= 200; // 0.0002 s
                    printf("%u\n\r", period); // DEBUG
                    break;
                case 'a': // turn off rgb
                    rgbDuty[RED] = OFF;
                    rgbDuty[GREEN] = OFF;
                    rgbDuty[BLUE] = OFF;
                    tour = false;
                    break;
                default: // if something unknown is hit, tell user
                    printf("Unknown key %c\r\n", cmd);
                    break;
            }
        }
    }
}

//// functions ////
void TMR0ISR(void)
{
    if(tour)
    {
        // set values:
        if(edgeLocation != END)
        {
            //printf("R:%u G:%u B:%u\n\rEdgeLocation: %u\n\r\n\r", rgbDuty[RED], rgbDuty[GREEN], rgbDuty[BLUE], edgeLocation); // DEBUG
            rgbDuty[RED] += rgbDirection[RED];
            rgbDuty[GREEN] += rgbDirection[GREEN];
            rgbDuty[BLUE] += rgbDirection[BLUE];
            edgeLocation++;
        }
        else
        {
            edgeLocation = START;
            tourDirectionIndex = (tourDirectionIndex + 1) % TOURNUMBER;
            rgbDirection[RED] = tourDirections[tourDirectionIndex][RED];
            rgbDirection[GREEN] = tourDirections[tourDirectionIndex][GREEN];
            rgbDirection[BLUE] = tourDirections[tourDirectionIndex][BLUE];
        }
    }
    // set PWM:
    EPWM1_LoadDutyValue(rgbDuty[RED]);
    EPWM2_LoadDutyValue(rgbDuty[GREEN]);
    EPWM3_LoadDutyValue(rgbDuty[BLUE]);
    // set timer:
    TMR0_WriteTimer(0x10000 - period);
    INTCONbits.TMR0IF = 0;
}