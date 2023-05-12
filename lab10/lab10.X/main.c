//--------------------------------------------------------------------
// Name:            Chris Coulston
// Date:            Fall 2020
// Purp:            inLab09
//
// Assisted:        The entire class of EENG 383
// Assisted by:     Microchips 18F26K22 Tech Docs 
//
// Academic Integrity Statement: I certify that, while others may have
// assisted me in brain storming, debugging and validating this program,
// the program itself is my own work. I understand that submitting code
// which is the work of other individuals is a violation of the course
// Academic Integrity Policy and may result in a zero credit for the
// assignment, or course failure and a report to the Academic Dishonesty
// Board. I also understand that if I knowingly give my original work to
// another individual that it could also result in a zero credit for the
// assignment, or course failure and a report to the Academic Dishonesty
// Board.
//------------------------------------------------------------------------

//////// includes ////////
#include "mcc_generated_files/mcc.h"
#include "sdCard.h"

//////// compiler ////////
#pragma warning disable 520     // warning: (520) function "xyz" is never called  3
#pragma warning disable 1498    // fputc.c:16:: warning: (1498) pointer (unknown)

//////// defines ////////
#define MEMSIZE 512
#define BLOCKSIZE 512
#define SAMPLEINC 160 // 10us in timer 0 counts
#define CSV ";"

//////// global variables ////////
uint16_t sampleRate = 1600; // 100us in timer 0 counts

uint8_t recording = false;
uint8_t playing = false;
uint8_t write = false;
uint8_t read = false;

uint8_t doubleBuffer[2][BLOCKSIZE];
uint8_t sampleIndex = 0;
uint8_t writeIndex = 1;
uint8_t setIndex = 0;
uint8_t readIndex = 1;
/*/
 *  Large arrays need to be defined as global even though you may only need to 
 *  use them in main. This is because automatic variables have a max size of 
 *  256 bytes.
/*/

////// function headers ////////
void rwTMR0ISR(void);

/////// main ////////
void main(void) 
{
    //// variables:
    char cmd;
    uint32_t address = 0; // SD card address
    uint32_t currentAddress = 0; // current SD card address
    uint16_t i = 0;
    uint16_t j = 0;
    uint8_t status = 0;
    const uint8_t sin[26] = {128, 159, 187, 213, 233, 248, 255, 255, 248, 233, 213, 187, 159, 128, 97, 69, 43, 23, 8, 1, 1, 8, 23, 43, 69, 97};
    uint8_t si = 0; // sin index
    //// initialize:
    SYSTEM_Initialize();
    CS_SetHigh();
    //// stablize baud rate:
    // provide baud rate generator time to stabilize before splash screen
    TMR0_WriteTimer(0x0000);
    INTCONbits.TMR0IF = 0;
    while (INTCONbits.TMR0IF == 0);
    //// enable interrupts:
    TMR0_SetInterruptHandler(rwTMR0ISR);
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
    //// restart SPI:
    SPI2_Close();
    SPI2_Open(SPI2_DEFAULT);
    //// print startup info:
    printf("\n\rinLab 09\r\n");
    printf("SD card testing\r\n");
    printf("Dev'21\r\n");
    printf("No configuration of development board\r\n> "); // print a nice command prompt    
    
    //// command line:
    for (;;) 
    {
        if (EUSART1_DataReady) 
        {
            cmd = EUSART1_Read(); // read data
            printf("%c\n\r", cmd); // print command
            //// execute command:
            switch (cmd)
            {
                //// help menu:
                case '?': // prints the help menu
                    printf("\r\n-------------------------------------------------\r\n");
                    printf("?: help menu\n\r");
                    printf("o: k\n\r");
                    printf("Z: Reset processor\n\r");
                    printf("z: Clear the terminal\n\r");
                    printf("-------------------------------------------------\n\r");
                    printf("i: Initialize SD card\n\r");
                    printf("-------------------------------------------------\n\r");
                    printf("a/A decrease/increase read address\n\r");
                    printf("r: read a block of 512 bytes from SD card\n\r");
                    printf("0: zero to 128 blocks\n\r");
                    printf("1: write perfect 26 value sine wave to 128 blocks\n\r");
                    printf("-------------------------------------------------\n\r");
                    printf("+/-: Increase/Decrease the sample rate by 10 us\n\r");
                    printf("R: Record microphone => SD card at 1600 us\n\r");
                    printf("P: Play from SD card to PWM -> LPF -> Audio\n\r");
                    printf("s: spool memory to a csv file\n\r");
                    printf("-------------------------------------------------\r\n");
                    break;
                //// ok test:
                case 'o': // reply with "k" (used for PC to PIC test)
                    printf("o:  ok\r\n");
                    break;
                //// reset processor:                   
                case 'Z': // reset the processor after clearing the terminal
                    for (i = 0; i < 40; i++) printf("\n");
                    RESET();
                    break;
                //// clear the terminal:
                case 'z': // clears the terminal by printing out a ton of spaces
                    for (i = 0; i < 40; i++) printf("\n");
                    break;
                //// initialize SD card:
                case 'i': // resets SPI and initializes the SD card (if read write before you will get caught in an infinte loop)
                    // restart SPI before using SPI with sd card:
                    SPI2_Close();
                    SPI2_Open(SPI2_DEFAULT);    // Reset the SPI channel for SD card communication
                    SDCARD_Initialize(true);
                    break;
                //// increase decrease address:
                case 'a':
                    //// decrement address:
                    address -= BLOCKSIZE;
                    if (address >= 0x04000000) 
                    {
                        printf("Underflowed to high address\r\n");
                        address = 0x04000000 - BLOCKSIZE;
                    } 
                    else 
                    {
                        printf("Decreased address\r\n");
                    }
                    //// print address:
                    // 32-bit integers need printed as a pair of 16-bit integers
                    printf("SD card address:  ");
                    printf("%04x", address >> 16);
                    printf(":");
                    printf("%04x", address & 0X0000FFFF);
                    printf("\r\n");
                    break;
                case 'A':
                    //// increment address:
                    address += BLOCKSIZE;
                    if (address >= 0x04000000) 
                    {
                        printf("Overflowed to low address\r\n");
                        address = 0x00000000;
                    } 
                    else 
                    {
                        printf("Increased address\r\n");
                    }
                    //// print address:
                    // 32-bit integers need printed as a pair of 16-bit integers
                    printf("SD card address:  ");
                    printf("%04x", address >> 16);
                    printf(":");
                    printf("%04x", address & 0X0000FFFF);
                    printf("\r\n");
                    break;
                //// read write to SD:
                case 'r': // read block of 512 bytes
                    READ_TIME_PIN_SetHigh();
                    SDCARD_ReadBlock(address, doubleBuffer[0]);
                    READ_TIME_PIN_SetLow();
                    printf("Read block: \r\n");
                    printf("    Address:    ");
                    printf("%04x", address >> 16);
                    printf(":");
                    printf("%04x", address & 0X0000FFFF);
                    printf("\r\n");
                    hexDumpBuffer(doubleBuffer[0]);
                    break;
                case 'w': // write block of 512 bytes
                    for (i = 0; i < BLOCKSIZE; i++) doubleBuffer[0][i] = 255 - i;
                    WRITE_TIME_PIN_SetHigh();
                    SDCARD_WriteBlock(address, doubleBuffer[0]);
                    while ((status = SDCARD_PollWriteComplete()) == WRITE_NOT_COMPLETE);
                    WRITE_TIME_PIN_SetLow();
                    
                    printf("Write block of decremented 8-bit values:\r\n");
                    printf("    Address:    ");
                    printf("%04x", address >> 16);
                    printf(":");
                    printf("%04x", address & 0X0000FFFF);
                    printf("\r\n");
                    printf("    Status:     %02x\r\n", status);
                    break;
                case '0': // write 128 blocks of 0 to the SD card
                    currentAddress = address;
                    printf("address: %u:%u", (address >> 16), (address & 0x0000ffff));
                    //// fill with zero:
                    for(i = 0; i < BLOCKSIZE; i++)
                    {
                        doubleBuffer[0][i] = 0;
                        si++;
                        if(si >= 26) si = 0; // wrap sin index around
                    }
                    //// write blocks:
                    for (i = 0; i < MEMSIZE; i++) 
                    {
                        //// write:
                        printf("write to "); printf("%04x:", currentAddress >> 16); printf("%04x ", currentAddress & 0x0000FFFF); // DEBUG
                        SDCARD_WriteBlock(currentAddress, doubleBuffer[0]);
                        //// wait while writing:
                        while(SDCARD_PollWriteComplete() == WRITE_NOT_COMPLETE); // wait while SD card is writing
                        TMR1_WriteTimer(0x0000);
                        PIR1bits.TMR1IF = 0;
                        while (PIR1bits.TMR1IF == 0);
                        ////
                        currentAddress += BLOCKSIZE; // increment address
                        printf("complete\n\r"); // DEBUG
                    }
                    printf("zeros written\n\r");
                    break;
                case '1': // write 128 blocks of perfect sign wave to the SD card
                    currentAddress = address;
                    //currentAddress = 0; // DEBUG
                    for (i = 0; i < MEMSIZE; i++) // loop through MEMSIZE blocks
                    {
                        //// fill block:
                        printf("write to "); printf("%04x:", currentAddress >> 16); printf("%04x ", currentAddress & 0x0000FFFF); // DEBUG
                        for(j = 0; j < BLOCKSIZE; j++)
                        {
                            doubleBuffer[0][j] = sin[si];
                            si++;
                            if(si >= 26) si = 0; // wrap sin index around
                        }
                        //// write block:
                        SDCARD_WriteBlock(currentAddress, doubleBuffer[0]);
                        //// wait finish writing:
                        while(SDCARD_PollWriteComplete() == WRITE_NOT_COMPLETE); // wait while SD card is writing
                        TMR1_WriteTimer(0x0000);
                        PIR1bits.TMR1IF = 0;
                        while (PIR1bits.TMR1IF == 0);
                        /*/
                         *  slows down the write sequence to insure that blocks are written.
                         *  it is unclear why this is needed but it bricks the SD otherwize.
                         *  maybe the SD card is still doing things after it has recived 
                         *  everything it is writing.
                        /*/
                        //// increment current address:
                        currentAddress += BLOCKSIZE;
                        printf("complete\n\r"); // DEBUG
                    }
                    printf("sin wave written\n\r");
                    break;
                //// increase decrease sample rate:
                case '+':
                    if((sampleRate + SAMPLEINC) > sampleRate) sampleRate += SAMPLEINC;
                    printf("sample time: %u counts\n\r", sampleRate);
                    break;
                case '-':
                    if((sampleRate - SAMPLEINC) < sampleRate) sampleRate -= SAMPLEINC;
                    printf("sample time: %u counts\n\r", sampleRate);
                    break;
                //// record (write audio) and playback (read audio):
                case 'R': // actively write audio to the SD card
                    //// setup:
                    write = false;
                    currentAddress = address;
                    //// record:
                    recording = true;
                    while(EUSART1_DataReady == false)
                    {
                        //// write:
                        if(write)
                        {
                            //// write block:
                            // printf("write doubleBuffer[%u] %04x:", writeIndex, currentAddress >> 16); printf("%04x\n\r", currentAddress & 0x0000FFFF); // DEBUG
                            SDCARD_WriteBlock(currentAddress, doubleBuffer[writeIndex]);
                            //// wait while writing:
                            while(SDCARD_PollWriteComplete() == WRITE_NOT_COMPLETE); // wait while SD card is writing
                            TMR1_WriteTimer(0x0000);
                            PIR1bits.TMR1IF = 0;
                            while (PIR1bits.TMR1IF == 0);
                            //// complete:
                            currentAddress += BLOCKSIZE;
                            write = false;
                        }
                    }
                    (void) EUSART1_Read();
                    //// stop recording:
                    recording = false;
                    printf("audio recorded\n\r");
                    break;
                case 'P': // actively write audio to the SD card
                    //// setup:
                    read = false;
                    currentAddress = address;
                    //// record:
                    playing = true;
                    while(EUSART1_DataReady == false)
                    {
                        //// read:
                        if(read)
                        {
                            //// read block:
                            //printf("read doubleBuffer[%u] %04x:", readIndex, currentAddress >> 16); printf("%04x\n\r", currentAddress & 0x0000FFFF); // DEBUG
                            SDCARD_ReadBlock(currentAddress, doubleBuffer[readIndex]);
                            //// complete:
                            currentAddress += BLOCKSIZE;
                            read = false;
                        }
                    }
                    (void) EUSART1_Read();
                    //// stop playing:
                    playing = false;
                    printf("audio played\n\r");
                    break;
                //// spool:
                case 's': // spool the SD card memory to a csv file
                    //// print instructions:
                    printf("You may terminate spooling at anytime with a keypress.\n\r");
                    printf("To spool terminal contents into a file follow these instructions:\n\r");
                    printf("\n\r");
                    printf("Right mouse click on the upper left of the PuTTY window\n\r");
                    printf("Select:     Change settings...\n\r");
                    printf("Select:     Logging\n\r");
                    printf("Select:     Session logging: All session output\n\r");
                    printf("Log file name:  Browse and provide a .csv extension to your file name\n\r");
                    printf("Select:     Apply\n\r");
                    printf("Press any key to start\n\r");
                    while(EUSART1_DataReady == false);
                    (void) EUSART1_Read();
                    //// print memory:
                    currentAddress = address;
                    for(i = 0; i < MEMSIZE; i++)
                    {
                        //// read block:
                        SDCARD_ReadBlock(currentAddress, doubleBuffer[0]);
                        //// print block:
                        for(j = 0; j < BLOCKSIZE; j++)
                        {
                            printf("%u%s", doubleBuffer[0][j], CSV); // print sample
                            if(EUSART1_DataReady == true) break;
                        }
                        if(EUSART1_DataReady == true) break;
                        //// increment current address:
                        currentAddress += BLOCKSIZE;
                    }
                    (void) EUSART1_Read();
                    //// print instructions:
                    printf("\n\rSpooled %u out of the %u blocks.\n\r", i, MEMSIZE);
                    printf("To close the file follow these instructions:\n\r");
                    printf("\n\r");
                    printf("Right mouse click on the upper left of the PuTTY window\n\r");
                    printf("Select:     Change settings...\n\r");
                    printf("Select:     Logging\n\r");
                    printf("Select:     Session  logging: None\n\r");
                    printf("Select:     Apply\n\r");
                    break;
                //// unknown:
                default:
                    printf("Unknown key %c\r\n", cmd);
                    break;
            }
            printf("> "); // print a nice command prompt
        } // end if
    } // end while 
} // end main

/*----------------------------------------------//
// As configured, we are hoping to get a toggle
// every 100us - this will require some work.
//
// You will be starting an ADC conversion here and
// storing the results (when you reenter) into a global
// variable and setting a flag, alerting main that 
// it can read a new value.
//
// !!!MAKE SURE THAT TMR0 has 0 TIMER PERIOD in MCC!!!!
//----------------------------------------------*/

//////// interupts ////////
void rwTMR0ISR(void) 
{
    //// variables:
    static uint16_t i = 0;
    uint8_t character;
    //// record:
    if(recording)
    {
        //// take sample:
        ADCON0bits.GO_NOT_DONE = 1; // trigger ADC to convert the current voltage to a value
        character = ADRESH;
        doubleBuffer[sampleIndex][i] = character; // record value
        // printf("%u;", character); // DEBUG
        i++; // increment index
        //// check buffer:
        if(i > BLOCKSIZE) // if index has surpassed block size
        {
            //// swap buffers:
            sampleIndex = !sampleIndex;
            writeIndex = !writeIndex;
            //// write buffer:
            write = true; // write block
            i = 0; // reset
        }
    }
    //// play:
    if(playing)
    {
        //// set pwm:
        // printf("%u%s", doubleBuffer[setIndex][i], CSV); // DEBUG
        EPWM1_LoadDutyValue(doubleBuffer[setIndex][i]);
        i++; // increment index
        //// check buffer:
        if(i > BLOCKSIZE)
        {
            //// swap buffers:
            setIndex = !setIndex;
            readIndex = !readIndex;
            //// read buffer:
            read = true; // read block
            i = 0; // reset
        }
    }
    ////
    TMR0_WriteTimer(0x10000 - (sampleRate - TMR0_ReadTimer()));
}
