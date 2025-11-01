#include "flash.h"

// Chip Select pin
#define FLASH_CS_PIN PA4
#define FLASH_SIZE_BYTES (16 * 1024 * 1024) // 16 MB total size


void flashInit() {
    // Configure CS pin
    pinMode(FLASH_CS_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH); // CS idle high

    // Initialize SPI with STM32 defaults
    
    // Max clock depends on your board, start with 10 MHz
    SPI.begin();
    
    
    // SPI_CLOCK_DIV16: Adjust to ~10 MHz
    // MSBFIRST: Most siginifact bit first
    // SPI_MODE0:  SPI mode 0: CPOL = 0, CPHA = 0

    SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV16,MSBFIRST,SPI_MODE0));
}


void flashRead(uint32_t addr, uint8_t* buffer, size_t len) {
    // Pull CS low to select flash
    digitalWrite(FLASH_CS_PIN, LOW);

    // Send Read command
    SPI.transfer(0x03);

    // Send 24-bit address (MSB first)
    SPI.transfer((addr >> 16) & 0xFF);
    SPI.transfer((addr >> 8) & 0xFF);
    SPI.transfer(addr & 0xFF);

    // Read 'len' bytes into buffer
    for (size_t i = 0; i < len; i++) {
        buffer[i] = SPI.transfer(0x00); // send dummy byte to clock in data
    }

    // Pull CS high to deselect flash
    digitalWrite(FLASH_CS_PIN, HIGH);
}


// Helper: send Write Enable command
void flashWriteEnable() {
    digitalWrite(FLASH_CS_PIN, LOW);
    SPI.transfer(0x06); // Write Enable
    digitalWrite(FLASH_CS_PIN, HIGH);
}

// Helper: check Busy bit in Status Register
bool flashIsBusy() {
    digitalWrite(FLASH_CS_PIN, LOW);
    SPI.transfer(0x05); // Read Status Register
    uint8_t status = SPI.transfer(0x00);
    digitalWrite(FLASH_CS_PIN, HIGH);
    return status & 0x01; // Busy bit = bit 0
}


void flashWrite(uint32_t addr, const uint8_t* data, size_t len) {
    size_t written = 0;

    while (written < len) {
        size_t pageOffset = addr % 256;              // position in page
        size_t chunk = min(256 - pageOffset, len - written); // bytes to write in this page

        flashWriteEnable(); // Enable writing

        digitalWrite(FLASH_CS_PIN, LOW);
        SPI.transfer(0x02); // Page Program command

        // Send 24-bit address
        SPI.transfer((addr >> 16) & 0xFF);
        SPI.transfer((addr >> 8) & 0xFF);
        SPI.transfer(addr & 0xFF);

        // Send data chunk
        for (size_t i = 0; i < chunk; i++) {
            SPI.transfer(data[written + i]);
        }
        digitalWrite(FLASH_CS_PIN, HIGH);

        // Wait until write completes
        while (flashIsBusy());

        // Move to next chunk/page
        addr += chunk;
        written += chunk;
    }
}

// Bulk erase entire chip
void flashEraseAll() {
    flashWriteEnable(); // initialise write to flash
    digitalWrite(FLASH_CS_PIN, LOW); // select flash
    SPI.transfer(0xC7); // full chip erase
    digitalWrite(FLASH_CS_PIN, HIGH); // deselect flash
    while (flashIsBusy()); // Wait for erase to complete
}


void flashDumpRange(uint32_t addr, size_t len) {
    const size_t chunkSize = 256;   // read in manageable chunks
    uint8_t buffer[chunkSize];

    while (len > 0) {
        size_t toRead = min(chunkSize, len);

        flashRead(addr, buffer, toRead);

        // Print as raw binary (can be piped to file later)
        for (size_t i = 0; i < toRead; i++) {
            Serial.write(buffer[i]);
        }

        addr += toRead;
        len  -= toRead;
    }
}

void flashDumpAll() {
    flashDumpRange(0, FLASH_SIZE_BYTES);
}
