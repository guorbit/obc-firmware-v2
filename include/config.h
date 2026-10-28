// Debug mode
constexpr bool OBC_DEBUG = false;
constexpr bool OBC_DEBUG_AS_COMMS = false; 


// Slow loop frequency
constexpr uint16_t SLOW_LOOP_FREQ = 15000; // e.g. 10000 = 10 sec

// Total length of each OBC message, in bytes
constexpr uint16_t OBC_MESSAGE_LEN = 512;
// User button
#define GPIO_USER_BUTTON PA_0

// Comms
#define GPIO_COMMS_CFG PC_6
constexpr uint16_t COMMS_CHUNK_INTERVAL_MILLIS = 3000;
constexpr uint8_t COMMS_CHUNK_SIZE = 58;


// ADCS
constexpr uint8_t ADCS_I2C_ADDRESS = 0x08;
constexpr uint8_t READOUT_LENGTH_ADCS = 148;
constexpr uint32_t ADCS_I2C_SPEED = 100000;
#define ADCS_I2C_SDA PB_7
#define ADCS_I2C_SCL PB_6


// Flash
#define FLASH_CS_PIN PA4   // Chip Select pin
constexpr uint8_t CMD_READ_DATA       = 0x03;
constexpr uint8_t CMD_PAGE_PROGRAM    = 0x02;
constexpr uint8_t CMD_WRITE_ENABLE    = 0x06; 
constexpr uint8_t CMD_WRITE_DISABLE   = 0x04;
constexpr uint8_t CMD_READ_STATUS     = 0x05;
constexpr uint8_t CMD_SECTOR_ERASE_4K = 0x20;


// SaveState
// 16 MB total size
constexpr uint32_t FLASH_SIZE_BYTES = 16 * 1024 * 1024;

// 4 KB reserved for metadata (tally bits)
constexpr uint16_t FLASH_METADATA_SIZE = 0x1000;

// Block size of user data per tally bit (adjustable)
constexpr uint16_t FLASH_BLOCK_SIZE = 512;

// First usable user data address (after metadata region)
constexpr uint16_t FLASH_USER_START = FLASH_METADATA_SIZE;
