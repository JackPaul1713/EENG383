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

//// includes ////
#include <pic18f25k22.h>
#include "mcc_generated_files/mcc.h"

//// compiler ////
#pragma warning disable 520     // warning: (520) function "xyz" is never called  3
#pragma warning disable 1498    // fputc.c:16:: warning: (1498) pointer (unknown)

//// defines ////
#define NUMBSAMPLES 512
#define UPPERTRIGGER (128 + threshold)
#define LOWERTRIGGER (128 - threshold)
#define SAMPLECOUNTS 400 // timer counts per sample (25us)
#define SAMPLEPERIOD 25 // time per sample (25us)
#define SAMPLEFREQUENCY 1000000/SAMPLEPERIOD 

/*
SAMPLING RATE
settings
  clock: FOSC/4
  prescaler: 1:1
  counts: 2^16 (16bit)
counts
  countTime = 4/64000000 * 1/1 = 1/16000000 sec/clk
  countSpeed = 1 / countTime = 16000000
calculations
  period = 25us = 0.00025 / (1/16000000) = 400
*/

//// global variables ////
uint8_t samples[NUMBSAMPLES]; // audio sample (technicaly 512 samples)
uint16_t crossings[NUMBSAMPLES/2];
uint8_t ci = 0; // crossing index
// sample:
uint8_t sampling = false;
uint8_t sampled = false;
uint8_t threshold = 10;
typedef enum {MICIDLE, MICTRIGGER, MICACQUIRE} sampleState_t;

//// function headers ////
void INIT_PIC(void);
void sampleTMR0ISR(void);

//// main ////
void main(void) {
    // variables:
    uint16_t i; 
    uint8_t j;
    char cmd;
    
    uint8_t sample;
    uint16_t period = 0;
    uint16_t avgPeriod = 0;
    uint16_t totPeriod = 0;
    uint32_t frequency;
    // initialize:
    SYSTEM_Initialize();
    // stablise eusart:
    TMR0_WriteTimer(0x0000);
    INTCONbits.TMR0IF = 0;
    while (INTCONbits.TMR0IF == 0);
    // enable interrupts:
    TMR0_SetInterruptHandler(sampleTMR0ISR);
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    // print startup info:
    printf("inLab 08\r\n");
    printf("Microphone experiments\r\n");
    printf("Dev'21\r\n");
    printf("> "); // print a nice command prompt
    // command line:
    for (;;) 
    {
        if (EUSART1_DataReady) // if there is incoming data on eusart
        {
            cmd = EUSART1_Read(); // read data
            printf("%c\n\r", cmd); // print command
            switch (cmd) // execute command
            {
                case '?': // print help menu
                    printf("------------------------------\r\n");
                    printf("?: Help menu\r\n");
                    printf("o: k\r\n");
                    printf("Z: Reset processor\r\n");
                    printf("z: Clear the terminal\r\n");
                    printf("T: increase sampling threshold\n\r");
                    printf("t: decrease sampling threshold\n\r");
                    printf("f: compute the period from %d samples from ADC\r\n", NUMBSAMPLES);
                    printf("s: gather %d samples from ADC\r\n", NUMBSAMPLES);
                    printf("0-9: switch sampled channel to ANx\r\n");
                    printf("------------------------------\r\n");
                    break;
                case 'o': // reply with "k" (used for PC to PIC test)
                    printf(" k\r\n");
                    break;             
                case 'Z': // reset the processor after clearing the terminal
                    for (i = 0; i < 40; i++) printf("\n\r");
                    RESET();
                    break;                    
                case 'z': // clear the terminal
                    for (i = 0; i < 40; i++) printf("\n\r");
                    break;
                case 'T':
                    threshold += 5;
                    printf("upper trigger: %u\n\r", UPPERTRIGGER);
                    printf("lower trigger: %u\n\r", LOWERTRIGGER);
                    break;
                case 't':
                    threshold -= 5;
                    printf("upper trigger: %u\n\r", UPPERTRIGGER);
                    printf("lower trigger: %u\n\r", LOWERTRIGGER);
                    break;
                case 'f':
                    // take sample:
                    {
                        printf("taking samples: \n\r");
                        sampling = true;
                        for(i = 0; i < NUMBSAMPLES; i++)
                        {
                            while(sampled == false);
                            samples[i] = ADRESH;
                            sampled = false;
                            // printf("sample[%u]%u\n\r", i, samples[i]); // DEBUG
                        }
                        sampling = false;
                        // for(i = 0; i < NUMBSAMPLES; i++) samples[i] = samples[i-1] + 4; // DEBUG
                    }
                    // find crossings:
                    {
                        printf("finding crossings: \n\r"); // DEBUG
                        ci = 0;
                        for(i = 1; i < NUMBSAMPLES; i++)
                        {
                            if((samples[i-1] <= 128) && (samples[i] > 128)) //|| ((samples[i-1] > 128) && (samples[i] <= 128)))
                            {
                                // printf("crossing[%u, %u]: %u\n\r", samples[i-1], samples[i], i); // DEBUG
                                crossings[ci] = i;
                                ci++;
                            }
                        }
                    }
                    // print sample:
                    {
                        printf("The last 256 ADC samples from the microphone are:\n\r");
                        for(i = 0; i < NUMBSAMPLES/16; i++)
                        {
                            printf("S[%u] ", i * 16); // print section
                            if(i*16 < 100) printf(" "); if(i*16 < 10) printf(" "); // ajust spacing
                            for(j = 0; j < 16; j++)
                            {
                                sample = samples[(i * 16) + j];
                                printf("%u ", sample); // print sample
                                if(sample < 100) printf(" "); if(sample < 10) printf(" "); // ajust spacing
                            }
                            printf("\n\r");
                        }
                        printf("\n\r");
                    }
                    // print crossings:
                    {
                        printf("The sound wave crossed at the following indicies:\n\r");
                        for(i = 0; i < ci; i++) // for every crossing
                        {
                            printf("%u ", crossings[i]); // print crossing
                        }
                        printf("\n\r\n\r");
                    }
                    // print periods:
                    {
                        printf("The sound wave had %u periods:\n\r", ci);
                        totPeriod = 0;
                        for(i = 1; i < ci; i++)
                        {
                            period = crossings[i] - crossings[i-1]; // calculate period
                            totPeriod += period; // add period to average period
                            printf("%u - %u = %u\n\r", crossings[i], crossings[i-1], period); // print period
                        }
                        printf("\n\r\n\r");
                        avgPeriod = (totPeriod * SAMPLEPERIOD) / (ci-1);
                        frequency = ((ci-1) * SAMPLEFREQUENCY) / totPeriod; // 1000000 / avgPeriod;
                        // print period and frequency:
                        printf("average period: %u us\n\r", avgPeriod);
                        printf("average frequency: %u Hz\n\r", frequency);
                    }
                    break;                     
                case '\0':
                    break;
                default: // if something is unknown, tell user
                    printf("unknown key\r\n", cmd);
                    break;
            }
            printf("> "); // print a nice command prompt
        }
    }
}

//// interrupts ////
void sampleTMR0ISR(void) // why does this ISR even exist if we need to wait while it's running?
{
    //TMR0_WriteTimer(0x10000);
    // variables:
    static sampleState_t state = MICIDLE;
    // sample:
    TEST_PIN_SetHigh(); // set high when we enter ISR
    ADCON0bits.GO_NOT_DONE = 1; // trigger ADC to convert the current voltage to a value
    switch(state)
    {
        case MICIDLE:
            if(sampling == true) state = MICTRIGGER;
            break;
        case MICTRIGGER:
            if((ADRESH > UPPERTRIGGER) || (ADRESH < LOWERTRIGGER)) state = MICACQUIRE;
            break;
        case MICACQUIRE:
            if(sampling == false) state = MICIDLE;
            sampled = true;
            break;
    }
    // set timer:
    TMR0_WriteTimer(0x10000 - (SAMPLECOUNTS - 75)); // TMR0_ReadTimer()));
    TEST_PIN_SetLow(); // set low when we enter ISR, monitor pulse width to determine how long we are in ISR
}