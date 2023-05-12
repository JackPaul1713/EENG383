void SDCARD_ReadBlock(uint32_t addr, uint8_t sdCardBuffer[]);
void SDCARD_WriteBlock(uint32_t addr, uint8_t sdCardBuffer[]);
void SDCARD_Initialize(uint8_t verbose);
void SDCARD_SetIdle(uint8_t verbose);
uint8_t SDCARD_SetBlockLength(void);
uint8_t SDCARD_PollWriteComplete(void);

void hexDumpBuffer(uint8_t sdCardBuffer[]);

#define CMD_GO_IDLE_STATE       0
#define CMD_SEND_OP_COND        1
#define CMD_SEND_STATUS         13
#define CMD_SET_BLOCK_LENGTH    16
#define CMD_READ_BLOCK          17
#define CMD_WRITE_BLOCK         24
#define START_TOKEN             0xFE
#define WRITE_NOT_COMPLETE      0xFF