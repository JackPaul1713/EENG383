/**
  @Generated Pin Manager Header File

  @Company:
    Microchip Technology Inc.

  @File Name:
    pin_manager.h

  @Summary:
    This is the Pin Manager file generated using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for .
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F25K22
        Driver Version    :  2.11
    The generated drivers are tested against the following:
        Compiler          :  XC8 2.36 and above
        MPLAB 	          :  MPLAB X 6.00	
*/

/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
*/

#ifndef PIN_MANAGER_H
#define PIN_MANAGER_H

/**
  Section: Included Files
*/

#include <xc.h>

#define INPUT   1
#define OUTPUT  0

#define HIGH    1
#define LOW     0

#define ANALOG      1
#define DIGITAL     0

#define PULL_UP_ENABLED      1
#define PULL_UP_DISABLED     0

// get/set channel_AN0 aliases
#define channel_AN0_TRIS                 TRISAbits.TRISA0
#define channel_AN0_LAT                  LATAbits.LATA0
#define channel_AN0_PORT                 PORTAbits.RA0
#define channel_AN0_ANS                  ANSELAbits.ANSA0
#define channel_AN0_SetHigh()            do { LATAbits.LATA0 = 1; } while(0)
#define channel_AN0_SetLow()             do { LATAbits.LATA0 = 0; } while(0)
#define channel_AN0_Toggle()             do { LATAbits.LATA0 = ~LATAbits.LATA0; } while(0)
#define channel_AN0_GetValue()           PORTAbits.RA0
#define channel_AN0_SetDigitalInput()    do { TRISAbits.TRISA0 = 1; } while(0)
#define channel_AN0_SetDigitalOutput()   do { TRISAbits.TRISA0 = 0; } while(0)
#define channel_AN0_SetAnalogMode()      do { ANSELAbits.ANSA0 = 1; } while(0)
#define channel_AN0_SetDigitalMode()     do { ANSELAbits.ANSA0 = 0; } while(0)

// get/set TEST_PIN aliases
#define TEST_PIN_TRIS                 TRISAbits.TRISA6
#define TEST_PIN_LAT                  LATAbits.LATA6
#define TEST_PIN_PORT                 PORTAbits.RA6
#define TEST_PIN_SetHigh()            do { LATAbits.LATA6 = 1; } while(0)
#define TEST_PIN_SetLow()             do { LATAbits.LATA6 = 0; } while(0)
#define TEST_PIN_Toggle()             do { LATAbits.LATA6 = ~LATAbits.LATA6; } while(0)
#define TEST_PIN_GetValue()           PORTAbits.RA6
#define TEST_PIN_SetDigitalInput()    do { TRISAbits.TRISA6 = 1; } while(0)
#define TEST_PIN_SetDigitalOutput()   do { TRISAbits.TRISA6 = 0; } while(0)

// get/set RB1 procedures
#define RB1_SetHigh()            do { LATBbits.LATB1 = 1; } while(0)
#define RB1_SetLow()             do { LATBbits.LATB1 = 0; } while(0)
#define RB1_Toggle()             do { LATBbits.LATB1 = ~LATBbits.LATB1; } while(0)
#define RB1_GetValue()              PORTBbits.RB1
#define RB1_SetDigitalInput()    do { TRISBbits.TRISB1 = 1; } while(0)
#define RB1_SetDigitalOutput()   do { TRISBbits.TRISB1 = 0; } while(0)
#define RB1_SetPullup()             do { WPUBbits.WPUB1 = 1; } while(0)
#define RB1_ResetPullup()           do { WPUBbits.WPUB1 = 0; } while(0)
#define RB1_SetAnalogMode()         do { ANSELBbits.ANSB1 = 1; } while(0)
#define RB1_SetDigitalMode()        do { ANSELBbits.ANSB1 = 0; } while(0)

// get/set RB2 procedures
#define RB2_SetHigh()            do { LATBbits.LATB2 = 1; } while(0)
#define RB2_SetLow()             do { LATBbits.LATB2 = 0; } while(0)
#define RB2_Toggle()             do { LATBbits.LATB2 = ~LATBbits.LATB2; } while(0)
#define RB2_GetValue()              PORTBbits.RB2
#define RB2_SetDigitalInput()    do { TRISBbits.TRISB2 = 1; } while(0)
#define RB2_SetDigitalOutput()   do { TRISBbits.TRISB2 = 0; } while(0)
#define RB2_SetPullup()             do { WPUBbits.WPUB2 = 1; } while(0)
#define RB2_ResetPullup()           do { WPUBbits.WPUB2 = 0; } while(0)
#define RB2_SetAnalogMode()         do { ANSELBbits.ANSB2 = 1; } while(0)
#define RB2_SetDigitalMode()        do { ANSELBbits.ANSB2 = 0; } while(0)

// get/set RB3 procedures
#define RB3_SetHigh()            do { LATBbits.LATB3 = 1; } while(0)
#define RB3_SetLow()             do { LATBbits.LATB3 = 0; } while(0)
#define RB3_Toggle()             do { LATBbits.LATB3 = ~LATBbits.LATB3; } while(0)
#define RB3_GetValue()              PORTBbits.RB3
#define RB3_SetDigitalInput()    do { TRISBbits.TRISB3 = 1; } while(0)
#define RB3_SetDigitalOutput()   do { TRISBbits.TRISB3 = 0; } while(0)
#define RB3_SetPullup()             do { WPUBbits.WPUB3 = 1; } while(0)
#define RB3_ResetPullup()           do { WPUBbits.WPUB3 = 0; } while(0)
#define RB3_SetAnalogMode()         do { ANSELBbits.ANSB3 = 1; } while(0)
#define RB3_SetDigitalMode()        do { ANSELBbits.ANSB3 = 0; } while(0)

// get/set CS aliases
#define CS_TRIS                 TRISBbits.TRISB4
#define CS_LAT                  LATBbits.LATB4
#define CS_PORT                 PORTBbits.RB4
#define CS_WPU                  WPUBbits.WPUB4
#define CS_ANS                  ANSELBbits.ANSB4
#define CS_SetHigh()            do { LATBbits.LATB4 = 1; } while(0)
#define CS_SetLow()             do { LATBbits.LATB4 = 0; } while(0)
#define CS_Toggle()             do { LATBbits.LATB4 = ~LATBbits.LATB4; } while(0)
#define CS_GetValue()           PORTBbits.RB4
#define CS_SetDigitalInput()    do { TRISBbits.TRISB4 = 1; } while(0)
#define CS_SetDigitalOutput()   do { TRISBbits.TRISB4 = 0; } while(0)
#define CS_SetPullup()          do { WPUBbits.WPUB4 = 1; } while(0)
#define CS_ResetPullup()        do { WPUBbits.WPUB4 = 0; } while(0)
#define CS_SetAnalogMode()      do { ANSELBbits.ANSB4 = 1; } while(0)
#define CS_SetDigitalMode()     do { ANSELBbits.ANSB4 = 0; } while(0)

// get/set WRITE_TIME_PIN aliases
#define WRITE_TIME_PIN_TRIS                 TRISCbits.TRISC4
#define WRITE_TIME_PIN_LAT                  LATCbits.LATC4
#define WRITE_TIME_PIN_PORT                 PORTCbits.RC4
#define WRITE_TIME_PIN_ANS                  ANSELCbits.ANSC4
#define WRITE_TIME_PIN_SetHigh()            do { LATCbits.LATC4 = 1; } while(0)
#define WRITE_TIME_PIN_SetLow()             do { LATCbits.LATC4 = 0; } while(0)
#define WRITE_TIME_PIN_Toggle()             do { LATCbits.LATC4 = ~LATCbits.LATC4; } while(0)
#define WRITE_TIME_PIN_GetValue()           PORTCbits.RC4
#define WRITE_TIME_PIN_SetDigitalInput()    do { TRISCbits.TRISC4 = 1; } while(0)
#define WRITE_TIME_PIN_SetDigitalOutput()   do { TRISCbits.TRISC4 = 0; } while(0)
#define WRITE_TIME_PIN_SetAnalogMode()      do { ANSELCbits.ANSC4 = 1; } while(0)
#define WRITE_TIME_PIN_SetDigitalMode()     do { ANSELCbits.ANSC4 = 0; } while(0)

// get/set READ_TIME_PIN aliases
#define READ_TIME_PIN_TRIS                 TRISCbits.TRISC5
#define READ_TIME_PIN_LAT                  LATCbits.LATC5
#define READ_TIME_PIN_PORT                 PORTCbits.RC5
#define READ_TIME_PIN_ANS                  ANSELCbits.ANSC5
#define READ_TIME_PIN_SetHigh()            do { LATCbits.LATC5 = 1; } while(0)
#define READ_TIME_PIN_SetLow()             do { LATCbits.LATC5 = 0; } while(0)
#define READ_TIME_PIN_Toggle()             do { LATCbits.LATC5 = ~LATCbits.LATC5; } while(0)
#define READ_TIME_PIN_GetValue()           PORTCbits.RC5
#define READ_TIME_PIN_SetDigitalInput()    do { TRISCbits.TRISC5 = 1; } while(0)
#define READ_TIME_PIN_SetDigitalOutput()   do { TRISCbits.TRISC5 = 0; } while(0)
#define READ_TIME_PIN_SetAnalogMode()      do { ANSELCbits.ANSC5 = 1; } while(0)
#define READ_TIME_PIN_SetDigitalMode()     do { ANSELCbits.ANSC5 = 0; } while(0)

// get/set RC6 procedures
#define RC6_SetHigh()            do { LATCbits.LATC6 = 1; } while(0)
#define RC6_SetLow()             do { LATCbits.LATC6 = 0; } while(0)
#define RC6_Toggle()             do { LATCbits.LATC6 = ~LATCbits.LATC6; } while(0)
#define RC6_GetValue()              PORTCbits.RC6
#define RC6_SetDigitalInput()    do { TRISCbits.TRISC6 = 1; } while(0)
#define RC6_SetDigitalOutput()   do { TRISCbits.TRISC6 = 0; } while(0)
#define RC6_SetAnalogMode()         do { ANSELCbits.ANSC6 = 1; } while(0)
#define RC6_SetDigitalMode()        do { ANSELCbits.ANSC6 = 0; } while(0)

// get/set RC7 procedures
#define RC7_SetHigh()            do { LATCbits.LATC7 = 1; } while(0)
#define RC7_SetLow()             do { LATCbits.LATC7 = 0; } while(0)
#define RC7_Toggle()             do { LATCbits.LATC7 = ~LATCbits.LATC7; } while(0)
#define RC7_GetValue()              PORTCbits.RC7
#define RC7_SetDigitalInput()    do { TRISCbits.TRISC7 = 1; } while(0)
#define RC7_SetDigitalOutput()   do { TRISCbits.TRISC7 = 0; } while(0)
#define RC7_SetAnalogMode()         do { ANSELCbits.ANSC7 = 1; } while(0)
#define RC7_SetDigitalMode()        do { ANSELCbits.ANSC7 = 0; } while(0)

/**
   @Param
    none
   @Returns
    none
   @Description
    GPIO and peripheral I/O initialization
   @Example
    PIN_MANAGER_Initialize();
 */
void PIN_MANAGER_Initialize (void);

/**
 * @Param
    none
 * @Returns
    none
 * @Description
    Interrupt on Change Handling routine
 * @Example
    PIN_MANAGER_IOC();
 */
void PIN_MANAGER_IOC(void);



#endif // PIN_MANAGER_H
/**
 End of File
*/