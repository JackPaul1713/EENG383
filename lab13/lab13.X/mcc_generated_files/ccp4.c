/**
  CCP4 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    ccp4.c

  @Summary
    This is the generated driver implementation file for the CCP4 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This source file provides implementations for driver APIs for CCP4.
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

/**
  Section: Included Files
*/

#include <xc.h>
#include "ccp4.h"

static void (*CCP4_CallBack)(uint16_t);

/**
  Section: Capture Module APIs:
*/

static void CCP4_DefaultCallBack(uint16_t capturedValue)
{
    // Add your code here
}

void CCP4_Initialize(void)
{
    // Set the CCP4 to the options selected in the User Interface
	
	// CCP4M Rising edge; DC4B 0; 
	CCP4CON = 0x05;    
	
	// CCPR4L 0; 
	CCPR4L = 0x00;    
	
	// CCPR4H 0; 
	CCPR4H = 0x00;    
    
    // Set the default call back function for CCP4
    CCP4_SetCallBack(CCP4_DefaultCallBack);

	// Selecting Timer 1
	CCPTMRS1bits.C4TSEL = 0x0;
    
    // Clear the CCP4 interrupt flag
    PIR4bits.CCP4IF = 0;

    // Enable the CCP4 interrupt
    PIE4bits.CCP4IE = 1;
}

void CCP4_CaptureISR(void)
{
    CCP4_PERIOD_REG_T module;

    // Clear the CCP4 interrupt flag
    PIR4bits.CCP4IF = 0;
    
    // Copy captured value.
    module.ccpr4l = CCPR4L;
    module.ccpr4h = CCPR4H;
    
    // Return 16bit captured value
    CCP4_CallBack(module.ccpr4_16Bit);
}

void CCP4_SetCallBack(void (*customCallBack)(uint16_t)){
    CCP4_CallBack = customCallBack;
}
/**
 End of File
*/