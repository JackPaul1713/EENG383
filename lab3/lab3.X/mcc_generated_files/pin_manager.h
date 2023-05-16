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

// get/set TOP_BUTTON aliases
#define TOP_BUTTON_TRIS                 TRISAbits.TRISA2
#define TOP_BUTTON_LAT                  LATAbits.LATA2
#define TOP_BUTTON_PORT                 PORTAbits.RA2
#define TOP_BUTTON_ANS                  ANSELAbits.ANSA2
#define TOP_BUTTON_SetHigh()            do { LATAbits.LATA2 = 1; } while(0)
#define TOP_BUTTON_SetLow()             do { LATAbits.LATA2 = 0; } while(0)
#define TOP_BUTTON_Toggle()             do { LATAbits.LATA2 = ~LATAbits.LATA2; } while(0)
#define TOP_BUTTON_GetValue()           PORTAbits.RA2
#define TOP_BUTTON_SetDigitalInput()    do { TRISAbits.TRISA2 = 1; } while(0)
#define TOP_BUTTON_SetDigitalOutput()   do { TRISAbits.TRISA2 = 0; } while(0)
#define TOP_BUTTON_SetAnalogMode()      do { ANSELAbits.ANSA2 = 1; } while(0)
#define TOP_BUTTON_SetDigitalMode()     do { ANSELAbits.ANSA2 = 0; } while(0)

// get/set BOTTOM_BUTTON aliases
#define BOTTOM_BUTTON_TRIS                 TRISAbits.TRISA3
#define BOTTOM_BUTTON_LAT                  LATAbits.LATA3
#define BOTTOM_BUTTON_PORT                 PORTAbits.RA3
#define BOTTOM_BUTTON_ANS                  ANSELAbits.ANSA3
#define BOTTOM_BUTTON_SetHigh()            do { LATAbits.LATA3 = 1; } while(0)
#define BOTTOM_BUTTON_SetLow()             do { LATAbits.LATA3 = 0; } while(0)
#define BOTTOM_BUTTON_Toggle()             do { LATAbits.LATA3 = ~LATAbits.LATA3; } while(0)
#define BOTTOM_BUTTON_GetValue()           PORTAbits.RA3
#define BOTTOM_BUTTON_SetDigitalInput()    do { TRISAbits.TRISA3 = 1; } while(0)
#define BOTTOM_BUTTON_SetDigitalOutput()   do { TRISAbits.TRISA3 = 0; } while(0)
#define BOTTOM_BUTTON_SetAnalogMode()      do { ANSELAbits.ANSA3 = 1; } while(0)
#define BOTTOM_BUTTON_SetDigitalMode()     do { ANSELAbits.ANSA3 = 0; } while(0)

// get/set IO_RA4 aliases
#define IO_RA4_TRIS                 TRISAbits.TRISA4
#define IO_RA4_LAT                  LATAbits.LATA4
#define IO_RA4_PORT                 PORTAbits.RA4
#define IO_RA4_SetHigh()            do { LATAbits.LATA4 = 1; } while(0)
#define IO_RA4_SetLow()             do { LATAbits.LATA4 = 0; } while(0)
#define IO_RA4_Toggle()             do { LATAbits.LATA4 = ~LATAbits.LATA4; } while(0)
#define IO_RA4_GetValue()           PORTAbits.RA4
#define IO_RA4_SetDigitalInput()    do { TRISAbits.TRISA4 = 1; } while(0)
#define IO_RA4_SetDigitalOutput()   do { TRISAbits.TRISA4 = 0; } while(0)

// get/set SPEAKER_PIN aliases
#define SPEAKER_PIN_TRIS                 TRISBbits.TRISB5
#define SPEAKER_PIN_LAT                  LATBbits.LATB5
#define SPEAKER_PIN_PORT                 PORTBbits.RB5
#define SPEAKER_PIN_WPU                  WPUBbits.WPUB5
#define SPEAKER_PIN_ANS                  ANSELBbits.ANSB5
#define SPEAKER_PIN_SetHigh()            do { LATBbits.LATB5 = 1; } while(0)
#define SPEAKER_PIN_SetLow()             do { LATBbits.LATB5 = 0; } while(0)
#define SPEAKER_PIN_Toggle()             do { LATBbits.LATB5 = ~LATBbits.LATB5; } while(0)
#define SPEAKER_PIN_GetValue()           PORTBbits.RB5
#define SPEAKER_PIN_SetDigitalInput()    do { TRISBbits.TRISB5 = 1; } while(0)
#define SPEAKER_PIN_SetDigitalOutput()   do { TRISBbits.TRISB5 = 0; } while(0)
#define SPEAKER_PIN_SetPullup()          do { WPUBbits.WPUB5 = 1; } while(0)
#define SPEAKER_PIN_ResetPullup()        do { WPUBbits.WPUB5 = 0; } while(0)
#define SPEAKER_PIN_SetAnalogMode()      do { ANSELBbits.ANSB5 = 1; } while(0)
#define SPEAKER_PIN_SetDigitalMode()     do { ANSELBbits.ANSB5 = 0; } while(0)

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