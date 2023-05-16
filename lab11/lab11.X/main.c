//--------------------------------------------------------------------
// Name:            Jack Martin, Jack Marley
// Date:            4/10/2023
// Purp:            direct digital synthisis
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
//////// includes ////////
#include "mcc_generated_files/mcc.h"

//////// compiler ////////
#pragma warning disable 520 // warning: (520) function "xyz" is never called 3
#pragma warning disable 1498 // fputc.c:16:: warning: (1498) pointer (unknown)

//////// defines ////////
#define ROMLENGTH 64 // size of read only memory for the sign wave, add an extra array entry for interpolation
#define UPDATEFUDGE 276
#define UPDATERATE 488 - UPDATEFUDGE // 488 // timer counts per update
/*/
 *  update rate:
 *  ?sec/upd = 2sec / (64/2^-10)mem/inc
 *  30.52us/upd
 *  488tmr/upd = 30.52E-6sec/upd / ((4/64E6)sec/clk * (1/1)clk/tmr)
/*/
#define PHASEINC 1
/*/
 *  phase increment:
 *  0'b000000.0000000001inc = 2^-10inc
/*/

//////// global variables ////////
uint8_t gensin = false; // generate sin wave
uint8_t sin[ROMLENGTH] = {128, 140, 152, 165, 176, 188, 198, 208, 218, 226, 234, 240, 245, 250, 253, 254, 255, 254, 253, 250, 245, 240, 234, 226, 218, 208, 198, 188, 176, 165, 152, 140, 128, 115, 103, 90, 79, 67, 57, 47, 37, 29, 21, 15, 10, 5, 2, 1, 0, 1, 2, 5, 10, 15, 21, 29, 37, 47, 57, 67, 79, 90, 103, 115}; // sin wave
uint8_t phase; // sin wave index, 6 bit number (2^6 = 64)
uint16_t phaseIncrement = PHASEINC; // 6:10 bit fixed point number
uint16_t phaseAccumulator = 0; // sum of phase increments over updates, 6:10 bit fixed point number
uint8_t amplitude = 16;

//////// function headers ////////
void TMR0ISR();
uint16_t read16bit(uint16_t maxValue);

//////// main ////////
void main(void) 
{
    //// variables:
    uint8_t i;
    char cmd;

    uint16_t startSweep = PHASEINC;
    uint16_t endSweep = 4096; // 2^16
    uint16_t sweepPeriod = 4000;
    uint16_t history;

    //// initialize system:
    SYSTEM_Initialize();

    //// stablize baud rate:
    // provide Baud rate generator time to stabilize before splash screen
    TMR0_WriteTimer(0x0000);
    INTCONbits.TMR0IF = 0;
    while(INTCONbits.TMR0IF == 0);

    //// enable interupts:
    INTERRUPT_GlobalInterruptEnable();    
    INTERRUPT_PeripheralInterruptEnable();
    TMR0_SetInterruptHandler(TMR0ISR);

    //// print startup info:
    printf("Lab 12\r\n");
    printf("DDS waveform generator\r\n");
    printf("Dev'21 Board wiring\r\n");
    printf("Connect jumper between RC2 and LPF_in.\r\n");
    printf("Connect jumper between LPF_out and AMP_in.\r\n");
    printf("Connect Channel 1 scope probe to RC2.\r\n");
    printf("Connect Channel 2 scope probe to LPF_out.\r\n");

    //// command line:
    printf("\n\r> "); // print a nice command prompt
    for (;;) 
    {
        if (EUSART1_DataReady) 
        {
            cmd = EUSART1_Read(); // read data
            printf("%c\n\r", cmd); // print command
            switch(cmd) // execute command
            {
                //// help menu:
                case '?': // prints the help menu
                    printf("-------------------------------------------------\n\r");
                    printf("--  Fixed\n\r");
                    printf("--      Frequency:  %uHz\n\r"), phaseIncrement;
                    printf("--      Amplitude:  %u/%u\n\r", 255);
                    printf("--  Sweep:\n\r");
                    printf("--      Starting:   %uHz\n\r", startSweep);
                    printf("--      Ending:     %uHz\n\r", endSweep);
                    printf("--      Duration:   %u second\n\r");
                    printf("-------------------------------------------------\n\r");
                    printf("?: help menu\n\r");
                    printf("o: k\n\r");
                    printf("Z: reset processor\n\r");
                    printf("z: clear the terminal\n\r");
                    printf("S/s: Start/stop fixed frequency sine wave\n\r");
                    printf("f: enter Frequency\n\r");
                    printf("A/a: increase/decrease Amplitude select amplitude\n\r");
                    printf("W: sWeep sine wave\n\r");
                    printf("t: enter sTarting frequency\n\r");
                    printf("e: enter Ending frequency\n\r");
                    printf("d: enter Duration\n\r");
                    printf("-------------------------------------------------\n\r");
                    break;
                //// ok test:
                case 'o': // reply with "k" (used for PC to PIC test)
                    printf("o:  ok\r\n");
                    break;
                //// reset processor:                   
                case 'Z': // reset the processor after clearing the terminal
                    for (i = 0; i < 40; i++) printf("\n");
                    RESET();
                    break;
                //// clear the terminal:
                case 'z': // clears the terminal by printing out a ton of spaces
                    for (i = 0; i < 40; i++) printf("\n");
                    break;

                //// generate a sin wave using DDS: 
                case 'S':
                    gensin = true;
                    printf("generating sin wave\n\r");
                    break;          
                case 's':
                    gensin = false;
                    printf("stopped generating sin wave\n\r");
                    break;
                case 'f':
                    printf("enter a frequency\n\r");
                    phaseIncrement = read16bit(32767) << 2;
                    printf(" Hz\n\r");
                    break;
                case 'a':
                    printf("enter an amplitude\n\r");
                    amplitude = read16bit(16);
                    printf("/16 steps\n\r");
                    break;

                //// generate sweeping frequency:
                case 'w':
                    history = phaseIncrement;
                    phaseAccumulator = 0;
                    printf("generating sweeping wave");
                    printf("press any key to exit.\r\n");
                    while (EUSART1_DataReady == false) 
                    {
                        for (phaseIncrement = startSweep; phaseIncrement < endSweep; phaseIncrement++) 
                        {
                            TMR1_WriteTimer(0x00 - sweepPeriod); 
                            PIR1bits.TMR1IF = 0;

                            gensin = true;
                            while (TMR1_HasOverflowOccured() == false);
                            gensin = false;
                        }
                    }
                    (void) EUSART1_Read();
                    phaseIncrement = history;
                    printf("stopped generating sweeping wave\r\n");
                    EPWM1_LoadDutyValue(0x80);
                    break;
                case 't': // change starting sweep frequency 
                    printf("enter a starting sweep frequency\n\r");
                    startSweep = read16bit(endSweep);
                    printf(" Hz\n\r");
                case 'e': // change ending sweep frequency
                    printf("enter a ending sweep frequency\n\r");
                    endSweep = read16bit(65535);
                    printf(" Hz\n\r");
                    break;
                case 'd': // change sweep period
                    printf("enter a sweep period\n\r");
                    sweepPeriod = read16bit(65535); 
                    printf(" tmrs\n\r");
                    break;
                //// unknown:
                default:
                    printf("unknown key %c\r\n", cmd);
                    break;
            }
            printf("> "); // print a nice command prompt
        }
    }
}

//////// interrupts ////////
void TMR0ISR()
{
    //// variables:
    uint8_t a0, a1, a2, a3, atot = 0;
    //// generate sin:
    if(gensin)
    {
        //// calculate sin:
        phaseAccumulator += phaseIncrement; // increment phase accumulator
        phase = phaseAccumulator >> 10; // calculate phase (sin index)
        //// set duty cycle:
        //printf("%usin[%u] = %u/16 * %u\n\r", amplitude, phase, amplitude * sin[phase]); // DEBUG
        if(amplitude & 0b00010000) atot = amplitude;
        else
        {
            if(amplitude & 0b00001000) a0 += amplitude >> 1;
            if(amplitude & 0b00000100) a1 += amplitude >> 2;
            if(amplitude & 0b00000010) a2 += amplitude >> 3;
            if(amplitude & 0b00000001) a3 += amplitude >> 4;
        }
        EPWM1_LoadDutyValue(atot); // (((uint16_t)sin[phase]) * amplitude) >> 4
    }
    //// set timer:
    TMR0_WriteTimer(0x0000 - UPDATERATE);
    INTCONbits.TMR0IF = 0;
}

//////// functions ////////
uint16_t read16bit(uint16_t maxValue)
{
    //// variables:
    uint16_t numb = 0;
    uint8_t digit = 0;
    uint8_t character = 0;
    uint8_t i = 0;
    //// read 8 bit number:
    while(character != '\r')
    {
        //// read:
        character = EUSART1_Read(); // read character
        //// delete:
        if(character == 127) 
        {
            printf("%c", character); // unprint last character
            if(numb > 0) numb = (((uint32_t)numb) * 6554) >> 16; // unshift number (divide by 10)
            if(i > 0) i--; // decrement index
            continue;
        }
        //// calculate:
        digit = character - '0'; // convert character to digit
        if((digit > 9) || (numb * 10 + digit > maxValue)) continue; // ignore invalid digits
        if((numb == 0) && (digit == 0) && (i > 0)) continue; // ignore excess leading zeros
        if((numb == 0) && (i > 0)) printf("%c", 127); // remove leading zero
        printf("%c", character); // print character
        numb *= 10; // shift number
        numb += digit; // add digit to number
        i++; // increment index
    }
    //// return:
    return(numb);
}
