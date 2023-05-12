/**
  CCP4 Generated Driver API Header File

  @Company
    Microchip Technology Inc.

  @File Name
    ccp4.h

  @Summary
    This is the generated header file for the CCP4 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This header file provides APIs for driver for CCP4.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F25K22
        Driver Version    :  2.0.3
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

#ifndef CCP4_H
#define CCP4_H

/**
  Section: Included Files
*/

#include <xc.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus  // Provide C++ Compatibility

    extern "C" {

#endif

/** 
   Section: Data Type Definition
*/

/**
 @Summary
   Defines the values to convert from 16bit to two 8 bit and vice versa

 @Description
   This routine used to get two 8 bit values from 16bit also
   two 8 bit value are combine to get 16bit.

 Remarks:
   None
 */

typedef union CCPR4Reg_tag
{
   struct
   {
      uint8_t ccpr4l;
      uint8_t ccpr4h;
   };
   struct
   {
      uint16_t ccpr4_16Bit;
   };
} CCP4_PERIOD_REG_T ;

/**
  Section: Capture Module APIs
*/

/**
  @Summary
    Initializes the CCP4

  @Description
    This routine initializes the CCP4_Initialize.
    This routine must be called before any other CCP4 routine is called.
    This routine should only be called once during system initialization.

  @Preconditions
    None

  @Param
    None

  @Returns
    None

  @Comment
    

 @Example
    <code>
    CCP4_Initialize();
    </code>
 */
void CCP4_Initialize(void);

/**
  @Summary
    Determines the completion of the data captured.

  @Description
    This routine is used to determine if data capture is completed.
    When data capture is complete routine returns 1. It returns 0 otherwise.

  @Preconditions
    CCP4_Initialize()function should have been called before calling this function.

  @Returns
    Returns 1 if data capture is completed
    true - Indicates data capture complete
    false - Indicates data capture is not complete

  @Param
    None

  @Example
    <code>
    CCP4_Initialize();
    while(!CCP4_IsCapturedDataReady());
    </code>
*/
bool CCP4_IsCapturedDataReady(void);

/**
  @Summary
    Reads the 16 bit capture value.

  @Description
    This routine reads the 16 bit capture value.

  @Preconditions
    CCP4_Initialize() and
    CCP4_IsCapturedDataReady() function should have been
    called before calling this function.

  @Param
    None.

  @Returns
    Returns 16 bit captured value

  @Example
    <code>
    uint16_t capture;

    CCP4_Initialize();

    while(!CCP4_IsCapturedDataReady());   //Used only for polling method
    capture = CCP4_CaptureRead();
    </code>
*/
uint16_t CCP4_CaptureRead(void);

#ifdef __cplusplus  // Provide C++ Compatibility

    }

#endif

#endif  //CCP4_H
/**
 End of File
*/

