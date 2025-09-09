#include "flash.h"

// Chip Select pin
#define FLASH_CS_PIN PA4
#define FLASH_SIZE_BYTES (16 * 1024 * 1024) // 16 MB total size
#define FLASH_METADATA_SIZE 0x100  // 256 bytes reserved for pointer etc.
#define FLASH_POINTER_ADDR   0x000000  // 4-byte "next free address" stored here
#define FLASH_USER_START 0x000004  // first address after metadata


void flashInit() {
    // Configure CS pin
    pinMode(FLASH_CS_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH); // CS idle high

    // Initialize SPI with STM32 defaults
    // SPI mode 0: CPOL = 0, CPHA = 0
    // Max clock depends on your board, start with 10 MHz
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV16); // Adjust to ~10 MHz
    SPI.setDataMode(SPI_MODE0);           // Mode 0
    SPI.setBitOrder(MSBFIRST);            // Most significant bit first
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
    // Automatically skip metadata region
    if (addr < FLASH_USER_START) {
        size_t offset = FLASH_USER_START - addr;
        if (len <= offset) {
            Serial.println("Warning: write request fully within metadata ignored.");
            return;  // all data was in metadata, nothing to write
        }
        addr = FLASH_USER_START;
        data += offset;
        len  -= offset;
    }

    size_t written = 0;
    while (written < len) {
        size_t pageOffset = addr % 256;
        size_t chunk = min(256 - pageOffset, len - written);

        flashWriteEnable();

        digitalWrite(FLASH_CS_PIN, LOW);
        SPI.transfer(0x02); // Page Program

        SPI.transfer((addr >> 16) & 0xFF);
        SPI.transfer((addr >> 8) & 0xFF);
        SPI.transfer(addr & 0xFF);

        for (size_t i = 0; i < chunk; i++) {
            SPI.transfer(data[written + i]);
        }
        digitalWrite(FLASH_CS_PIN, HIGH);

        while (flashIsBusy());

        addr += chunk;
        written += chunk;
    }
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



uint32_t flashGetNextFreeAddr() {
    uint8_t buf[4];  // 4 bytes for the pointer
    flashRead(FLASH_POINTER_ADDR, buf, 4);

    // Combine bytes into a uint32_t (big-endian: MSB first)
    uint32_t addr = ((uint32_t)buf[0] << 24) |
                    ((uint32_t)buf[1] << 16) |
                    ((uint32_t)buf[2] << 8)  |
                    ((uint32_t)buf[3]);

    // Sanity check: if pointer is invalid, reset to FLASH_USER_START
    if (addr < FLASH_USER_START || addr >= FLASH_SIZE_BYTES) {
        Serial.println("Warning: flash pointer invalid, resetting to FLASH_USER_START");
        addr = FLASH_USER_START;
    }

    return addr;
}


void flashSetNextFreeAddr(uint32_t addr) {
    // Sanity check: prevent writing outside flash bounds
    if (addr < FLASH_USER_START || addr >= FLASH_SIZE_BYTES) {
        Serial.println("Error: Attempt to set flash pointer outside valid range");
        return;
    }

    // Split uint32_t into 4 bytes (big-endian: MSB first)
    uint8_t buf[4];
    buf[0] = (addr >> 24) & 0xFF;
    buf[1] = (addr >> 16) & 0xFF;
    buf[2] = (addr >> 8) & 0xFF;
    buf[3] = addr & 0xFF;

    // Write the 4 bytes to the pointer address
    flashWrite(FLASH_POINTER_ADDR, buf, 4);
}

