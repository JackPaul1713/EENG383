//--------------------------------------------------------------------
// Name:            Jack Martin, Jack Marley
// Date:            Spring 2023
// Purp:            measure distance from echo pulse duration and ajust led color to reflect distance
//
// Assisted:        
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

/////// compiler ////////
#pragma warning disable 520     // warning: (520) function "xyz" is never called  3
#pragma warning disable 1498    // fputc.c:16:: warning: (1498) pointer (unknown)

//////// includes ////////
#include "mcc_generated_files/mcc.h"

//////// defines ////////
#define NUM_SAMPLES 8

#define RISE CCP4CONbits.CCP4M == 0b0101
#define FALL CCP4CONbits.CCP4M == 0b0100

//////// global variables ////////
uint16_t echo; // duration of echo
uint8_t newEcho; // echo collected

//////// function headers ////////
void TMR0ISR(void);
void CCP4ISR(uint16_t);
uint16_t convertEchToCm(uint16_t timerCounts);
void microSecondDelay(uint16_t us);

//////// main ////////
void main(void) 
{
    //// variables:
    char cmd;
    uint16_t sample[NUM_SAMPLES];
    uint16_t red; // red duty cycle steps
    uint16_t green; // green duty cycle steps
    uint8_t i;
    uint8_t j;
    uint16_t temp;
    uint8_t min;
    //// initialize:     
    SYSTEM_Initialize();
    //// stablize baud rate:
    // provide baud rate generator time to stabilize before splash screen
    TMR0_WriteTimer(0x0000);
    INTCONbits.TMR0IF = 0;
    while (INTCONbits.TMR0IF == 0);
    //// enable interupts:
    TMR0_SetInterruptHandler(TMR0ISR);
    CCP4_SetCallBack(CCP4ISR);
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    //// turn off leds:
    EPWM1_LoadDutyValue(256);
    EPWM2_LoadDutyValue(256);
    //// print startup info:
	printf("Dev'21 Board\r\n");
	printf("inLab 13 terminal\r\n");
    printf("Ultrasonic Connections:\r\n");
    printf("    Vcc: left pin on JP1 header (closest to mini USB).\r\n");
    printf("    Trig:   RC4\r\n");
    printf("    Echo:   RB0\r\n");
    printf("    GND:    GND header pin on PIC breakout header.\r\n");
	printf("\r\n> "); // print a nice command prompt
    //// command line:
	for(;;) 
    {
		if (EUSART1_DataReady) // wait for char on EUSART
        {
            cmd = EUSART1_Read(); // read data
            printf("%c\n\r", cmd); // print command
            //// execute command:
			switch (cmd) 
            {
                //// help menu:
                case '?':
                    printf("------------------------------\r\n");
                    printf("?: help menu\r\n");                   
                    printf("o: k\r\n");
                    printf("Z: reset processor\r\n");
                    printf("z: clear the terminal\r\n");
                    printf("s: stream Echo pulse length\r\n");  
                    printf("S: stream distance\r\n");             
                    printf("c: collect and sort distances\n\r"); 
                    printf("C: convert distances to color\n\r"); 
                    printf("------------------------------\r\n");                    
                    break;
                //// ok test: 
                case 'o':
                    printf(" k\r\n");
                    break;
                //// reset and clear terminal:                 
                case 'Z':
                    for (i = 0; i < 40; i++) printf("\n");
                    RESET();
                    break;
                //// clear terminal:                
                case 'z':
                    for (i = 0; i < 40; i++) printf("\n>");
                    break;
                //// stream echo pulse durations (timer counts):                   
                case 's':
                    printf("press any key to exit\r\n");
                    newEcho = false;
                    while(!EUSART1_DataReady) 
                    {                        
                        if(newEcho)
                        {
                            printf("%8u\r", echo);
                            newEcho = false;
                        }
                    }
                    (void) EUSART1_Read();
                    printf("\r\n");
                    break;
                //// stream echo pulse durations (cm):                             
                case 'S':
                    printf("press any key to exit\r\n");
                    newEcho = false;
                    while(!EUSART1_DataReady)
                    {         
                        if(newEcho)
                        {
                            printf("%8u\r", convertEchToCm(echo));
                            newEcho = false;
                        }               
                    }
                    (void) EUSART1_Read();
                    printf("\r\n");
                    break;                    
                //// collect sample size samples, display, sort, and display them:             
                case 'c':
                    //// collect samples:
                    newEcho = false;
                    for (i = 0; i < NUM_SAMPLES; i++) 
                    {
                        while(newEcho == false);
                        sample[i] = echo;
                        newEcho = false;
                    }
                    //// print samples:
                    printf("Unsorted\r\n");
                    for (i = 0; i < NUM_SAMPLES; i++) printf("%u  ", sample[i]);
                    printf("\r\n");
                    //// sort samples:
                    for (i = 0; i < NUM_SAMPLES; i++)
                    {
                        //// find minimum between i and sample size:
                        min = i;
                        for (j = i; j < NUM_SAMPLES; j++) 
                        {
                            if(sample[j] < sample[min]) min = j;
                        }
                        //// swap positions:
                        temp = sample[i];
                        sample[i] = sample[min];
                        sample[min] = temp;
                    }
                    //// print samples:
                    printf("Sorted\r\n");
                    for (i = 0; i < NUM_SAMPLES; i++) printf("%u  ", sample[i]);
                    printf("\r\n");
                    break;                    
                //// colors:
                case 'C':
                    printf("press any key to exit\r\n");
                    newEcho = false;
                    while(!EUSART1_DataReady)
                    {
                        if(newEcho)
                        {
                            /*/
                             *  maximum timer counts:
                             *    17724 timer counts in 148cm (maximum distance) 
                             *  minimum timer counts:
                             *    2395 timer counts in 20cm (minimum distance)
                             *  timer counts per step:
                             *    60 timer counts per step
                             *    60 = (max timer counts - min timer counts) / 256
                             *    steps = x / 60 = x * 1111 >> 16 = x >> 6
                            /*/
                            //// calculate steps:
                            green = echo >> 6; // convert timer counts to steps
                            red = 256 - green; // flip green steps for red
                            if(green > 256) {red = 0; green = 256;} // cap green at 256
                            printf("\rred(%3u), green(%3u)", red, green); // DEBUG
                            //// set duty cycle:
                            EPWM1_LoadDutyValue(red);
                            EPWM2_LoadDutyValue(green);
                            newEcho = false;
                        }                      
                    }
                    (void) EUSART1_Read();
                    EPWM1_LoadDutyValue(256);
                    EPWM2_LoadDutyValue(256);
                    printf("\r\n");
                    break;  
                //// unknown key:
                default:
                    printf("Unknown key %c\r\n",cmd);
                    break;
			}
            printf("> "); // print a nice command prompt
		}
	}
}

//// interrupts ////
void TMR0ISR(void)
{
    uint16_t i;
    
    TRIGGER_PIN_SetHigh();  
    microSecondDelay(25);
    TRIGGER_PIN_SetLow();      
    
    TMR0_WriteTimer(0x10000 - 50000); // 100ms delay
}
void CCP4ISR(uint16_t capture)
{
    static uint16_t rise; // timer counts at rise
    static uint16_t fall; // timer counts at fall
    // printf("capture isr: "); // DEBUG
    if(RISE)
    {
        // printf("rise(%u)\n\r", capture); // DEBUG
        rise = capture; // set rise time
        CCP4CONbits.CCP4M = 0b0100; // set to capture on falling edge
    }
    else if(FALL)
    {
        // printf("fall(%u)\n\r", capture); // DEBUG
        fall = capture; // set fall time
        CCP4CONbits.CCP4M = 0b0101; // set to capture on rising edge

        echo = fall - rise; // calculate echo duration
        newEcho = true;
    }
    PIR4bits.CCP4IF = 0; // clear ccp4 flag
}

//// functions ////
/*/
 *  This routine uses the Capture module inside the CCP4 unit to look for
 *  a positive edge of the echo pulse on RB0.  When this happens, the 
 *  ECCP1_CaptureRead function return the timer 1 value when this happened.  
 *  The code then  configures itself to look for a falling edge of the 
 *  the RB0 pin.  When the negative edge occurs, the CCP module captures
 *  the timer 1 value in the CCPR1 register.  The subsequent call to
 *  ECCP1_CaptureRead returns this value into the end variable.  The difference
 *  between end and start is returned as the proxy for the distance.
 *
 *  You will need to convert this function into the CCP4_CallBack ISR.  Under 
 *  no circumstance are you to use a busy wait loop like:
 *           while(CCP4_IsCapturedDataReady() == false);
 *  in your ISR code.  Since the ISR is called on a negative or positive edge, 
 *  there is no reason to have the ISR wait, just have the ISR exit.  The ISR 
 *  will then be called when the edge occurs.  
/*/
uint16_t convertEchToCm(uint16_t timerCounts) 
{
    return((547 * ((uint32_t) timerCounts)) >> 16);
}

//////// wtf lol ////////
// Create a delay of 1uS and loop a number of times
void microSecondDelay(uint16_t us) 
{
    //// variables:
    uint16_t i;
    //// delay:
    for (i = 0; i < us; i++) 
    {
        asm("NOP"); // 1
        asm("NOP"); // 2
        asm("NOP"); // 3
        asm("NOP"); // 4
        asm("NOP"); // 5
        asm("NOP"); // 6
        i = i;
    }   
}