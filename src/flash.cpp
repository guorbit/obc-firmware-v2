#include "flash.h"

// -------------------- Pin and Command Setup --------------------

#define FLASH_CS_PIN PA4   // Chip Select pin
#define CMD_READ_DATA        0x03
#define CMD_PAGE_PROGRAM     0x02
#define CMD_WRITE_ENABLE     0x06
#define CMD_WRITE_DISABLE    0x04
#define CMD_READ_STATUS      0x05
#define CMD_SECTOR_ERASE_4K  0x20

// -------------------- Relevant Definitions --------------------

#define FLASH_SIZE_BYTES (16 * 1024 * 1024) // 16MB total chip size

// -------------------- Initialization --------------------

void flashInit() {
    // Configure CS pin
    pinMode(FLASH_CS_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH);

    // Initialize SPI with STM32 defaults
    
    // Max clock depends on your board, start with 10 MHz
    SPI.begin();
    
    
    // SPI_CLOCK_DIV16: Adjust to ~10 MHz
    // MSBFIRST: Most siginifact bit first
    // SPI_MODE0:  SPI mode 0: CPOL = 0, CPHA = 0

    SPI.beginTransaction(SPISettings(SPI_CLOCK_DIV16,MSBFIRST,SPI_MODE0));
}

// -------------------- Basic Operations --------------------

bool flashIsBusy() {
    digitalWrite(FLASH_CS_PIN, LOW);
    SPI.transfer(CMD_READ_STATUS);
    uint8_t status = SPI.transfer(0x00);
    digitalWrite(FLASH_CS_PIN, HIGH);
    return (status & 0x01); // Busy bit
}

void flashWriteEnable() {
    digitalWrite(FLASH_CS_PIN, LOW);
    SPI.transfer(CMD_WRITE_ENABLE);
    digitalWrite(FLASH_CS_PIN, HIGH);
}

void flashWriteDisable() {
    digitalWrite(FLASH_CS_PIN, LOW);
    SPI.transfer(CMD_WRITE_DISABLE);
    digitalWrite(FLASH_CS_PIN, HIGH);
}

void flashRead(uint32_t addr, uint8_t* buffer, size_t len) {
    digitalWrite(FLASH_CS_PIN, LOW);

    SPI.transfer(CMD_READ_DATA);
    SPI.transfer((addr >> 16) & 0xFF);
    SPI.transfer((addr >> 8) & 0xFF);
    SPI.transfer(addr & 0xFF);

    for (size_t i = 0; i < len; i++) {
        buffer[i] = SPI.transfer(0x00);
    }

    digitalWrite(FLASH_CS_PIN, HIGH);
}

void flashWrite(uint32_t addr, const uint8_t* data, size_t len) {
    size_t written = 0;

    while (written < len) {
        size_t pageOffset = addr % 256;
        size_t chunk = min(256 - pageOffset, len - written);

        flashWriteEnable();

        digitalWrite(FLASH_CS_PIN, LOW);
        SPI.transfer(CMD_PAGE_PROGRAM);
        SPI.transfer((addr >> 16) & 0xFF);
        SPI.transfer((addr >> 8) & 0xFF);
        SPI.transfer(addr & 0xFF);

        for (size_t i = 0; i < chunk; i++) {
            SPI.transfer(data[written + i]);
        }

        digitalWrite(FLASH_CS_PIN, HIGH);

        // Wait for write to complete
        while (flashIsBusy()) {
            // Busy wait
        }

        addr += chunk;
        written += chunk;
    }

    flashWriteDisable();
}

// -------------------- Erase --------------------

void flashEraseSector(uint32_t addr) {
    uint32_t sectorAddr = addr & 0xFFFFF000; // Align to 4KB

    flashWriteEnable();

    digitalWrite(FLASH_CS_PIN, LOW);
    SPI.transfer(CMD_SECTOR_ERASE_4K);
    SPI.transfer((sectorAddr >> 16) & 0xFF);
    SPI.transfer((sectorAddr >> 8) & 0xFF);
    SPI.transfer(sectorAddr & 0xFF);
    digitalWrite(FLASH_CS_PIN, HIGH);

    while (flashIsBusy()) {
        // Busy wait
    }

    flashWriteDisable();
}

// Bulk erase entire chip
void flashEraseAll() {
    flashWriteEnable(); // initialise write to flash
    digitalWrite(FLASH_CS_PIN, LOW); // select flash
    SPI.transfer(0xC7); // full chip erase
    digitalWrite(FLASH_CS_PIN, HIGH); // deselect flash
    while (flashIsBusy()); // Wait for erase to complete
}

// -------------------- Dump Utilities --------------------

void flashDumpRange(uint32_t addr, size_t len) {
    const size_t chunkSize = 256;
    uint8_t buffer[chunkSize];

    while (len > 0) {
        size_t toRead = min(chunkSize, len);
        flashRead(addr, buffer, toRead);

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
