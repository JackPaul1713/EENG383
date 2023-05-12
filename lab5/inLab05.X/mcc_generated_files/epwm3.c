/**
  EPWM3 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    epwm3.c

  @Summary
    This is the generated driver implementation file for the EPWM3 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This source file provides implementations for driver APIs for EPWM3.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F25K22
        Driver Version    :  2.01
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
#include "epwm3.h"

/**
  Section: Macro Declarations
*/

#define PWM3_INITIALIZE_DUTY_VALUE    127

/**
  Section: EPWM Module APIs
*/

void EPWM3_Initialize(void)
{
    // Set the EPWM3 to the options selected in the User Interface
	
	// CCP3M P3A,P3C: active high; P3B,P3D: active high; DC3B 3; P3M single; 
	CCP3CON = 0x3C;    
	
	// CCP3ASE operating; PSS3BD low; PSS3AC low; CCP3AS disabled; 
	ECCP3AS = 0x00;    
	
	// P3RSEN automatic_restart; P3DC 0; 
	PWM3CON = 0x80;    
	
	// STR3D P3D_to_port; STR3C P3C_to_port; STR3B P3B_to_port; STR3A P3A_to_CCP3M; STR3SYNC start_at_begin; 
	PSTR3CON = 0x01;    
	
	// CCPR3H 0; 
	CCPR3H = 0x00;    
	
	// CCPR3L 31; 
	CCPR3L = 0x1F;    

	// Selecting Timer2
	CCPTMRS0bits.C3TSEL = 0x0;
}

void EPWM3_LoadDutyValue(uint16_t dutyValue)
{
   // Writing to 8 MSBs of pwm duty cycle in CCPRL register
    CCPR3L = ((dutyValue & 0x03FC)>>2);
    
   // Writing to 2 LSBs of pwm duty cycle in CCPCON register
    CCP3CON = ((uint8_t)(CCP3CON & 0xCF) | ((dutyValue & 0x0003)<<4));
}
/**
 End of File
*/

