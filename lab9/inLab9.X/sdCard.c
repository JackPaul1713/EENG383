//--------------------------------------------------------------
// http://elm-chan.org/docs/mmc/mmc_e.html
//--------------------------------------------------------------

//// includes ////
#include "mcc_generated_files/mcc.h"
#include "sdCard.h"

//// compiler ////
#pragma warning disable 373

//// sd card functions ////
// initialize SPI on SD:
void SDCARD_Initialize(uint8_t verbose) 
{
    // variables:
    uint8_t response;

    // initialize SPI:
    // send sd card at least 74 clock cycles to SD card to tell it that we want to communicate using SPI
    for (int i = 0; i < 10; i++) 
    {
        SPI2_ExchangeByte(0xFF);
    }

    // idle processor:
    // put processor in IDLE state to reset the SD Card processor state
    CS_SetLow();
    SPI2_ExchangeByte(0xFF);
    SPI2_ExchangeByte(0x40 | CMD_GO_IDLE_STATE);
    SPI2_ExchangeByte(0x00);
    SPI2_ExchangeByte(0x00);
    SPI2_ExchangeByte(0x00);
    SPI2_ExchangeByte(0x00);
    SPI2_ExchangeByte(0x95);
    SPI2_ExchangeByte(0xFF);
    response = SPI2_ExchangeByte(0xFF);
    CS_SetHigh();
    if (verbose == true) printf("CMD0, Reset Response: %x\r\n", response);

    // operate processor:
    // put processor in OPERATION CONDITION state to run initialization sequence
    do 
    {
        CS_SetLow();
        SPI2_ExchangeByte(0xFF);
        SPI2_ExchangeByte(0x40 | CMD_SEND_OP_COND);
        SPI2_ExchangeByte(0x00);
        SPI2_ExchangeByte(0x00);
        SPI2_ExchangeByte(0x00);
        SPI2_ExchangeByte(0x00);
        SPI2_ExchangeByte(0xFF);
        SPI2_ExchangeByte(0xFF);
        response = SPI2_ExchangeByte(0xFF);
        CS_SetHigh();
    } 
    while (response != 0);
    if (verbose == true) printf("CMD1, Init Response: %x\r\n", response);

    // set block size 512:
    // define the number of bytes per block to equal 512
    do 
    {
        CS_SetLow();
        SPI2_ExchangeByte(0xFF);
        SPI2_ExchangeByte(0x40 | CMD_SET_BLOCK_LENGTH);
        SPI2_ExchangeByte(0x00);
        SPI2_ExchangeByte(0x00);
        SPI2_ExchangeByte(0x02);
        SPI2_ExchangeByte(0x00);
        SPI2_ExchangeByte(0xFF);
        SPI2_ExchangeByte(0xFF);
        response = SPI2_ExchangeByte(0xFF);
        CS_SetHigh();
    } 
    while (response == 0xFF);
    if (verbose == true) printf("Block Length Response: %x\r\n", response);
}
// read write 512 bytes:
void SDCARD_ReadBlock(uint32_t addr, uint8_t sdCardBuffer[]) // reads block (512 bytes) from input address
{
    // variables:
    uint16_t i = 0;

    // select sd card (SPI):
    CS_SetLow();
    // send read block command:
    SPI2_ExchangeByte(0xFF);
    SPI2_ExchangeByte(0x40 | CMD_READ_BLOCK); // send read block command to tell sd card to read block
    SPI2_ExchangeByte((uint8_t) ((addr >> 24) & 0xFF)); // send first 8 bits of address
    SPI2_ExchangeByte((uint8_t) ((addr >> 16) & 0xFF)); // send next 8 bits of address
    SPI2_ExchangeByte((uint8_t) ((addr >> 8) & 0xFF)); // send next 8 bits of address
    SPI2_ExchangeByte((uint8_t) (addr & 0xFF)); // send last 8 bits of address
    SPI2_ExchangeByte(0xFF);
    // wait for sd card:
    while (SPI2_ExchangeByte(0xFF) == 0xFF); // wait for R1 response
    while (SPI2_ExchangeByte(0xFF) == 0xFF); // wait for start token at the start of the data packet
    // read block (512 bytes):
    for (i = 0; i < 512; i++)
        sdCardBuffer[i] = SPI2_ExchangeByte(0xFF);
    SPI2_ExchangeByte(0xFF); // chew up the 2-byte CRC
    SPI2_ExchangeByte(0xFF); // chew up the 2-byte CRC
    // deselect sd card (SPI):
    CS_SetHigh(); 
}
void SDCARD_WriteBlock(uint32_t addr, uint8_t sdCardBuffer[]) // writes block (512 bytes) to input address
{
    // variables:
    uint16_t i = 0;

    // select sd card (SPI):
    CS_SetLow();
    // send write block command:
    SPI2_ExchangeByte(0xFF);
    SPI2_ExchangeByte(0x40 | CMD_WRITE_BLOCK);
    SPI2_ExchangeByte((uint8_t) ((addr >> 24) & 0xFF));
    SPI2_ExchangeByte((uint8_t) ((addr >> 16) & 0xFF));
    SPI2_ExchangeByte((uint8_t) ((addr >> 8) & 0xFF));
    SPI2_ExchangeByte((uint8_t) (addr & 0xFF));
    SPI2_ExchangeByte(0xFF);
    // wait for sd card:
    while (SPI2_ExchangeByte(0xFF) == 0xFF); // wait for R1 response
    // buffer:
    SPI2_ExchangeByte(0xFF); // send at least one byte to buffer data packet
    SPI2_ExchangeByte(0xFF); // (at least)
    SPI2_ExchangeByte(0xFF); // (at least)
    // write block (512 bytes):
    SPI2_ExchangeByte(START_TOKEN); // start data packet with a 1 byte data token
    for (i = 0; i < 512; i++) // 512 byte data block
        SPI2_ExchangeByte(sdCardBuffer[i]);
    // deselect sd card (SPI):
    CS_SetHigh();
}
// check if writing:
uint8_t SDCARD_PollWriteComplete(void) // checks if sd card is being writen to
{

    uint8_t status;

    CS_SetLow();
    status = SPI2_ExchangeByte(CMD_SEND_STATUS);
    CS_SetHigh();

    if (status == 0xFF) 
    {
        return (WRITE_NOT_COMPLETE);
    } 
    else 
    {
        // REad out all 32-bits
        (void) SPI2_ExchangeByte(0xFF);
        (void) SPI2_ExchangeByte(0xFF);
        (void) SPI2_ExchangeByte(0xFF);
        return (status);
    }
}

//// functions ////
// print 512 bytes:
void hexDumpBuffer(uint8_t sdCardBuffer[]) // prints 512 bytes from an array
{
    // variables:
    uint16_t i;
    uint16_t j;
    // print:
    printf("\r\n\n"); // leading new lines
    for (i = 0; i < 512; i++) 
    {
        if (i != 0 && i % 8 == 0) printf(" ");
        if (i != 0 && i % 16 == 0) 
        {
            printf("  ");
            for (j = i - 16; j < i; j++) 
            {
                if (sdCardBuffer[j] < 32) 
                {
                    printf(".");
                } 
                else 
                {
                    printf("%c", sdCardBuffer[j]);
                }
            }
            printf("\r\n");
        }
        printf("%02x ", sdCardBuffer[i]);
    }
    printf("   ");
    for (int j = i - 16; j < i; j++) 
    {
        if (sdCardBuffer[j] < 32) 
        {
            printf(".");
        } 
        else 
        {
            printf("%c", sdCardBuffer[j]);
        }
    }
    printf("\r\n");
}