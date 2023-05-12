/**
  EUSART2 Generated Driver File

  @Company
    Microchip Technology Inc.

  @File Name
    eusart2.c

  @Summary
    This is the generated driver implementation file for the EUSART2 driver using PIC10 / PIC12 / PIC16 / PIC18 MCUs

  @Description
    This source file provides APIs for EUSART2.
    Generation Information :
        Product Revision  :  PIC10 / PIC12 / PIC16 / PIC18 MCUs - 1.81.8
        Device            :  PIC18F25K22
        Driver Version    :  2.1.1
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
#include "eusart2.h"

/**
  Section: Macro Declarations
*/

#define EUSART2_TX_BUFFER_SIZE 8
#define EUSART2_RX_BUFFER_SIZE 8

/**
  Section: Global Variables
*/
volatile uint8_t eusart2TxHead = 0;
volatile uint8_t eusart2TxTail = 0;
volatile uint8_t eusart2TxBuffer[EUSART2_TX_BUFFER_SIZE];
volatile uint8_t eusart2TxBufferRemaining;

volatile uint8_t eusart2RxHead = 0;
volatile uint8_t eusart2RxTail = 0;
volatile uint8_t eusart2RxBuffer[EUSART2_RX_BUFFER_SIZE];
volatile eusart2_status_t eusart2RxStatusBuffer[EUSART2_RX_BUFFER_SIZE];
volatile uint8_t eusart2RxCount;
volatile eusart2_status_t eusart2RxLastError;

/**
  Section: EUSART2 APIs
*/
void (*EUSART2_TxDefaultInterruptHandler)(void);
void (*EUSART2_RxDefaultInterruptHandler)(void);

void (*EUSART2_FramingErrorHandler)(void);
void (*EUSART2_OverrunErrorHandler)(void);
void (*EUSART2_ErrorHandler)(void);

void EUSART2_DefaultFramingErrorHandler(void);
void EUSART2_DefaultOverrunErrorHandler(void);
void EUSART2_DefaultErrorHandler(void);

void EUSART2_Initialize(void)
{
    // disable interrupts before changing states
    PIE3bits.RC2IE = 0;
    EUSART2_SetRxInterruptHandler(EUSART2_Receive_ISR);
    PIE3bits.TX2IE = 0;
    EUSART2_SetTxInterruptHandler(EUSART2_Transmit_ISR);
    // Set the EUSART2 module to the options selected in the user interface.

    // ABDOVF no_overflow; CKTXP async_noninverted_sync_fallingedge; BRG16 16bit_generator; WUE disabled; ABDEN disabled; DTRXP not_inverted; 
    BAUDCON2 = 0x08;

    // SPEN enabled; RX9 8-bit; CREN enabled; ADDEN disabled; SREN disabled; 
    RCSTA2 = 0x90;

    // TX9 8-bit; TX9D 0; SENDB sync_break_complete; TXEN enabled; SYNC asynchronous; BRGH hi_speed; CSRC slave_mode; 
    TXSTA2 = 0x24;

    // 
    SPBRG2 = 0x14;

    // 
    SPBRGH2 = 0x34;


    EUSART2_SetFramingErrorHandler(EUSART2_DefaultFramingErrorHandler);
    EUSART2_SetOverrunErrorHandler(EUSART2_DefaultOverrunErrorHandler);
    EUSART2_SetErrorHandler(EUSART2_DefaultErrorHandler);

    eusart2RxLastError.status = 0;

    // initializing the driver state
    eusart2TxHead = 0;
    eusart2TxTail = 0;
    eusart2TxBufferRemaining = sizeof(eusart2TxBuffer);

    eusart2RxHead = 0;
    eusart2RxTail = 0;
    eusart2RxCount = 0;

    // enable receive interrupt
    PIE3bits.RC2IE = 1;
}

bool EUSART2_is_tx_ready(void)
{
    return (eusart2TxBufferRemaining ? true : false);
}

bool EUSART2_is_rx_ready(void)
{
    return (eusart2RxCount ? true : false);
}

bool EUSART2_is_tx_done(void)
{
    return TXSTA2bits.TRMT;
}

eusart2_status_t EUSART2_get_last_status(void){
    return eusart2RxLastError;
}

uint8_t EUSART2_Read(void)
{
    uint8_t readValue  = 0;
    
    while(0 == eusart2RxCount)
    {
    }

    eusart2RxLastError = eusart2RxStatusBuffer[eusart2RxTail];

    readValue = eusart2RxBuffer[eusart2RxTail++];
    if(sizeof(eusart2RxBuffer) <= eusart2RxTail)
    {
        eusart2RxTail = 0;
    }
    PIE3bits.RC2IE = 0;
    eusart2RxCount--;
    PIE3bits.RC2IE = 1;

    return readValue;
}

void EUSART2_Write(uint8_t txData)
{
    while(0 == eusart2TxBufferRemaining)
    {
    }

    if(0 == PIE3bits.TX2IE)
    {
        TXREG2 = txData;
    }
    else
    {
        PIE3bits.TX2IE = 0;
        eusart2TxBuffer[eusart2TxHead++] = txData;
        if(sizeof(eusart2TxBuffer) <= eusart2TxHead)
        {
            eusart2TxHead = 0;
        }
        eusart2TxBufferRemaining--;
    }
    PIE3bits.TX2IE = 1;
}


void EUSART2_Transmit_ISR(void)
{

    // add your EUSART2 interrupt custom code
    if(sizeof(eusart2TxBuffer) > eusart2TxBufferRemaining)
    {
        TXREG2 = eusart2TxBuffer[eusart2TxTail++];
        if(sizeof(eusart2TxBuffer) <= eusart2TxTail)
        {
            eusart2TxTail = 0;
        }
        eusart2TxBufferRemaining++;
    }
    else
    {
        PIE3bits.TX2IE = 0;
    }
}

void EUSART2_Receive_ISR(void)
{
    
    eusart2RxStatusBuffer[eusart2RxHead].status = 0;

    if(RCSTA2bits.FERR){
        eusart2RxStatusBuffer[eusart2RxHead].ferr = 1;
        EUSART2_FramingErrorHandler();
    }

    if(RCSTA2bits.OERR){
        eusart2RxStatusBuffer[eusart2RxHead].oerr = 1;
        EUSART2_OverrunErrorHandler();
    }
    
    if(eusart2RxStatusBuffer[eusart2RxHead].status){
        EUSART2_ErrorHandler();
    } else {
        EUSART2_RxDataHandler();
    }
    
    // or set custom function using EUSART2_SetRxInterruptHandler()
}

void EUSART2_RxDataHandler(void){
    // use this default receive interrupt handler code
    eusart2RxBuffer[eusart2RxHead++] = RCREG2;
    if(sizeof(eusart2RxBuffer) <= eusart2RxHead)
    {
        eusart2RxHead = 0;
    }
    eusart2RxCount++;
}

void EUSART2_DefaultFramingErrorHandler(void){}

void EUSART2_DefaultOverrunErrorHandler(void){
    // EUSART2 error - restart

    RCSTA2bits.CREN = 0;
    RCSTA2bits.CREN = 1;

}

void EUSART2_DefaultErrorHandler(void){
    EUSART2_RxDataHandler();
}

void EUSART2_SetFramingErrorHandler(void (* interruptHandler)(void)){
    EUSART2_FramingErrorHandler = interruptHandler;
}

void EUSART2_SetOverrunErrorHandler(void (* interruptHandler)(void)){
    EUSART2_OverrunErrorHandler = interruptHandler;
}

void EUSART2_SetErrorHandler(void (* interruptHandler)(void)){
    EUSART2_ErrorHandler = interruptHandler;
}

void EUSART2_SetTxInterruptHandler(void (* interruptHandler)(void)){
    EUSART2_TxDefaultInterruptHandler = interruptHandler;
}

void EUSART2_SetRxInterruptHandler(void (* interruptHandler)(void)){
    EUSART2_RxDefaultInterruptHandler = interruptHandler;
}
/**
  End of File
*/
