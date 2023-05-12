//*****************************************************************
// Name:    Dr. Chris Coulston
// Date:    Aug 31, 2017
// Lab:     02
//
// Purp:    Blinking a message in morse code with an LED
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

//// initialize ////
#include <xc.h>
#include <stdint.h>             // look @ \Microchip\xc8\v1.38\include
#include <stdbool.h>
#include <stdio.h>

// Configuration bits
#pragma config FOSC = INTIO67    // Oscillator Selection bits->Internal oscillator block
#pragma config PLLCFG = OFF    // 4X PLL Enable->Oscillator used directly
#pragma config PRICLKEN = ON    // Primary clock enable bit->Primary clock enabled

void initPIC(void);
uint8_t convert(char letter);
void blink(char letter);
void milliSecondDelay(uint16_t ms);
void microSecondDelay(uint16_t us);

#define BUTTON_PIN  PORTAbits.RA2       // always use "PORT" for inputs
#define BUTTON_TRIS TRISAbits.TRISA2    // Make your code self-documenting
#define BUTTON_ANG  ANSELAbits.ANSA2    // needed for digital input

#define LED_PIN     LATBbits.LATB5      // always use "LAT" for outputs
#define LED_TRIS    TRISBbits.TRISB5
#define LED_ANG     ANSELBbits.ANSB5

#define INPUT       1                   // Make code self-documenting
#define OUTPUT      0                   // Page 135 of PIC18(L)F2X/4XK22 Data Sheet

#define ANALOG      1                   // Page PIC18(L)F2X/4XK22 Data Sheet
#define DIGITAL     0

#define DOT 0
#define DASH 1
#define END 2

#define MESSAGE "sos help"
#define LENGTH 9

//// main ////
void main(void) 
{
    // variables:
    char message[LENGTH] = MESSAGE;
    // initialize micro controller:
    initPIC();
    // transmit:
    for (;;)
    {
        // turn off LED:
        LED_PIN = 1;
        // wait button press:
        while(BUTTON_PIN == 1); // While button is not pressed, wait
        while(BUTTON_PIN == 0); // The button is being held down
        // blink message:
        for(int i = 0; i < LENGTH; i++) blink(message[i]);
    }
}

//// PIC18 ////
void initPIC(void)
{
    // ---------------Configure Oscillator------------------
    OSCCONbits.IRCF2 = 1; // Internal RC Oscillator Frequency Select bits
    OSCCONbits.IRCF1 = 1; // Set to 16Mhz
    OSCCONbits.IRCF0 = 1; //
    OSCTUNEbits.PLLEN = 1; // enable the 4xPLL, wicked fast 64Mhz

    BUTTON_ANG = DIGITAL; // Must do for any input which is multiplex with ADC
    BUTTON_TRIS = INPUT; // initialize DDR bit makes push button an input

    LED_ANG = DIGITAL; // Not really needed because LED is an output
    LED_TRIS = OUTPUT; // initialize DDR bit makes LED an output
}

//// functions ////
uint8_t convert(char letter)
{
    // variables:
    uint8_t index = letter - 97;
    // check:
    if(index > 26) return(255);
    // return:
    return(index);
}
void blink(char letter)
{
    // variables:
    uint8_t i;
    uint8_t j;
    uint8_t morseCode[26][5] = {
        {DOT, DASH, END}, // a
        {DASH, DOT, DOT, DOT, END}, // b
        {DASH, DOT, DASH, DOT, END}, // c
        {DASH, DOT, DOT, END}, // d
        {DOT, END}, // e
        {DOT, DOT, DASH, DOT, END}, // f
        {DASH, DASH, DOT, END}, // g
        {DOT, DOT, DOT, DOT, END}, // h
        {DOT, DOT, END}, // i
        {DOT, DASH, DASH, DASH, END}, // j
        {DASH, DOT, DASH, END}, // k
        {DOT, DASH, DOT, DOT, END}, // l
        {DASH, DASH, END}, // m
        {DASH, DOT, END}, // n
        {DASH, DASH, DASH, END}, // o
        {DOT, DASH, DASH, DOT, END}, // p
        {DASH, DASH, DOT, DASH, END}, // q
        {DOT, DASH, DOT, END}, // r
        {DOT, DOT, DOT, END}, // s
        {DASH, END}, // t
        {DOT, DOT, DASH, END}, // u
        {DOT, DOT, DOT, DASH, END}, // v
        {DOT, DASH, DASH, END}, // w
        {DASH, DOT, DOT, DASH, END}, // x
        {DASH, DOT, DASH, DASH, END}, // y
        {DASH, DASH, DOT, DOT, END}}; // z
    uint8_t timeUnit = 200;
    // convert:
    i = convert(letter);
    j = 0;
    // check word end:
    if(i == 255)
    {
       milliSecondDelay(timeUnit * 4); // word delay
       return;
    }
    // blink letter:
    while(morseCode[i][j] != END)
    {
        if(morseCode[i][j] == DOT)
        {
            LED_PIN = 0;
            milliSecondDelay(timeUnit * 1); // dot delay
            LED_PIN = 1;
        }
        else if(morseCode[i][j] == DASH)
        {
            LED_PIN = 0;
            milliSecondDelay(timeUnit * 3); // dash delay
            LED_PIN = 1;
        }
        // else break; UNESSISARY
        milliSecondDelay(timeUnit * 1); // IMU delay
        j++;
    }
    milliSecondDelay(timeUnit * 2); // letter delay (includes an IMU)
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
