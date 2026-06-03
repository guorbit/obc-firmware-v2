// Debug mode
#define OBC_DEBUG false

// Slow loop frequency
#define SLOW_LOOP_FREQ 10000 // e.g. 10000 = 10 sec

// Total length of each OBC message, in bytes
#define OBC_MESSAGE_LEN 512

// User button
#define GPIO_USER_BUTTON PA_0

// Comms
#define GPIO_COMMS_CFG PC_6


// ADCS
#define ADCS_I2C_ADDRESS 0x08
#define READOUT_LENGTH_ADCS 142
#define ADCS_I2C_SPEED 100000
#define ADCS_I2C_SDA PB_7
#define ADCS_I2C_SCL PB_6


// Flash
#define FLASH_CS_PIN PA4   // Chip Select pin
#define CMD_READ_DATA        0x03
#define CMD_PAGE_PROGRAM     0x02
#define CMD_WRITE_ENABLE     0x06
#define CMD_WRITE_DISABLE    0x04
#define CMD_READ_STATUS      0x05
#define CMD_SECTOR_ERASE_4K  0x20


// SaveState
// 16 MB total size
#define FLASH_SIZE_BYTES (16 * 1024 * 1024)

// 4 KB reserved for metadata (tally bits)
#define FLASH_METADATA_SIZE 0x1000

// Block size of user data per tally bit (adjustable)
#define FLASH_BLOCK_SIZE 512

// First usable user data address (after metadata region)
#define FLASH_USER_START FLASH_METADATA_SIZE
