//--------------------------------------------------------------------
// Name:            Chris Coulston
// Date:            Fall 2020
// Purp:            inLab13
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
#pragma warning disable 520     // warning: (520) function "xyz" is never called  3
#pragma warning disable 1498    // fputc.c:16:: warning: (1498) pointer (unknown)

#include "mcc_generated_files/mcc.h"

void      myTMR0ISR(void);
uint16_t  convertEchToCm(uint16_t timerCounts);
uint16_t  getEchoPulseDurationForeground(void);
void      microSecondDelay(uint16_t us);

#define NUM_SAMPLES 8

//----------------------------------------------
// Main "function"
//----------------------------------------------
void main (void) {

    uint16_t    sample[NUM_SAMPLES];
    uint8_t     i;
    char        cmd;
            
    SYSTEM_Initialize();
    
    // BEFORE enabling interrupts, otherwise that while loop becomes an
    // infinite loop.  Doing this to give EUSART1's baud rate generator time
    // to stablize - this will make the splash screen looks better
    TMR0_WriteTimer(0x0000);
    INTCONbits.TMR0IF = 0;
    while (INTCONbits.TMR0IF == 0);
    
    TMR0_SetInterruptHandler(myTMR0ISR);
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
        
	printf("Dev'21 Board\r\n");
	printf("inLab 13 terminal\r\n");
    printf("Ultrasonic Connections:\r\n");
    printf("    Vcc: left pin on JP1 header (closest to mini USB).\r\n");
    printf("    Trig:   RC4\r\n");
    printf("    Echo:   RB0\r\n");
    printf("    GND:    GND header pin on PIC breakout header.\r\n");
	printf("\r\n> ");                       // print a nice command prompt

	for(;;) {

		if (EUSART1_DataReady) {			// wait for incoming data on USART
            cmd = EUSART1_Read();
			switch (cmd) {		// and do what it tells you to do

                case '?':
                    printf("------------------------------\r\n");
                    printf("?: Help menu\r\n");                   
                    printf("o: k\r\n");
                    printf("Z: Reset processor\r\n");
                    printf("z: Clear the terminal\r\n");
                    printf("s: Stream Echo pulse length\r\n");  
                    printf("S: Stream distance\r\n");             
                    printf("a: gather %d samples in an array and print\r\n",NUM_SAMPLES); 
                    printf("------------------------------\r\n");                    
                    break;
                    
                //--------------------------------------------
                // Reply with "k", used for PC to PIC test
                //--------------------------------------------    
                case 'o':
                    printf(" k\r\n");
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
                    for (i = 0; i < 40; i++) printf("\n>");
                    break;
    
                //--------------------------------------------
                // Stream echo pulse duration in timer counts
                //--------------------------------------------                      
                case 's':
                    printf("press any key to exit\r\n");
                    while(!EUSART1_DataReady) {                        
                        printf("%8u\r",getEchoPulseDurationForeground());
                    }
                    (void) EUSART1_Read();
                    printf("\r\n");
                    break;

                //--------------------------------------------
                // Stream echo pulse duration in timer counts
                //--------------------------------------------                      
                case 'S':
                    printf("press any key to exit\r\n");
                    while(!EUSART1_DataReady) {                        
                        printf("%8u\r",convertEchToCm(getEchoPulseDurationForeground()));
                    }
                    (void) EUSART1_Read();
                    printf("\r\n");
                    break;                    
                    
                //--------------------------------------------
                // Acquire N samples and display them
                //--------------------------------------------                      
                case 'a':                    
                    for (i=0; i<NUM_SAMPLES; i++) {
                        sample[i] = getEchoPulseDurationForeground();
                    }
                    printf("%d samples\r\n",NUM_SAMPLES);
                    for (i=0; i<NUM_SAMPLES; i++) printf("%u  ",sample[i]);
                    printf("\r\n");
                    break;                    
    
                //--------------------------------------------
                // If something unknown is hit, tell user
                //--------------------------------------------
                default:
                    printf("Unknown key %c\r\n",cmd);
                    break;
			} // end switch
            
		}	// end if
	} // end while 
} // end main

//------------------------------------------------------------------------
// This routine uses the Capture module inside the CCP4 unit to look for
// a positive edge of the echo pulse on RB0.  When this happens, the 
// ECCP1_CaptureRead function return the timer 1 value when this happened.  
// The code then  configures itself to look for a falling edge of the 
// the RB0 pin.  When the negative edge occurs, the CCP module captures
// the timer 1 value in the CCPR1 register.  The subsequent call to
// ECCP1_CaptureRead returns this value into the end variable.  The difference
// between end and start is returned as the proxy for the distance.
//
// You will need to convert this function into the CCP4_CallBack ISR.  Under 
// no circumstance are you to use a busy wait loop like:
//          while(CCP4_IsCapturedDataReady() == false);
// in your ISR code.  Since the ISR is called on a negative or positive edge, 
// there is no reason to have the ISR wait, just have the ISR exit.  The ISR 
// will then be called when the edge occurs.  
//------------------------------------------------------------------------
uint16_t getEchoPulseDurationForeground(void) {

    uint16_t start, end;
    
    CCP4CONbits.CCP4M = 0b0101;                     // CCP1 => Capture on rising edge
    PIR4bits.CCP4IF = 0;                            // Clear flag
    while(CCP4_IsCapturedDataReady() == false);     // Wait for rising edge
    start = CCP4_CaptureRead();

    CCP4CONbits.CCP4M = 0b0100;                     // CCP1 => Capture on falling edge 
    PIR4bits.CCP4IF = 0;
    while(CCP4_IsCapturedDataReady() == false);
    end = CCP4_CaptureRead();
    
    return(end - start);
}

//------------------------------------------------------------------------
// Hold the trigger high for 25us, once every 100ms
//------------------------------------------------------------------------
void myTMR0ISR(void){

    uint16_t i;
    
    TRIGGER_PIN_SetHigh();	
    microSecondDelay(10);
    TRIGGER_PIN_SetLow();	   
    
    TMR0_WriteTimer(0x10000 - 50000);   // 100ms delay

} // end 

//------------------------------------------------------------------------
//------------------------------------------------------------------------
uint16_t convertEchToCm(uint16_t timerCounts) {
    return(timerCounts >> 7);
}


//*****************************************************************
// Create a delay of 1uS and loop a number of times
//*****************************************************************

void microSecondDelay(uint16_t us) {

    uint16_t i;

    for (i = 0; i < us; i++) {
        asm("NOP"); // 1
        asm("NOP"); // 2
        asm("NOP"); // 3
        asm("NOP"); // 4
        asm("NOP"); // 5
        asm("NOP"); // 6
        i = i;
    } // end for     
} // end microSecond()