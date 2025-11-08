#include "flash.h"

// -------------------- Pin and Command Setup --------------------

#define FLASH_CS_PIN PA4   // Chip Select pin
#define CMD_READ_DATA        0x03
#define CMD_PAGE_PROGRAM     0x02
#define CMD_WRITE_ENABLE     0x06
#define CMD_WRITE_DISABLE    0x04
#define CMD_READ_STATUS      0x05
#define CMD_SECTOR_ERASE_4K  0x20

// -------------------- Initialization --------------------

void flashInit() {
    // Configure CS pin
    pinMode(FLASH_CS_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH);

    // Setup SPI
    SPI.begin();
    SPI.setDataMode(SPI_MODE0);
    SPI.setBitOrder(MSBFIRST);
    SPI.setClockDivider(SPI_CLOCK_DIV16); // ~10 MHz depending on board

    // ---- Ensure metadata sector is erased (initialized to 0xFF) ----
    uint8_t metaCheck;
    flashRead(0x000000, &metaCheck, 1);

    if (metaCheck != 0xFF) {
        Serial.println("Metadata sector not empty. Erasing...");
        flashEraseSector(0x000000);
        Serial.println("Metadata sector initialized to all 1s.");
    } else {
        Serial.println("Metadata sector OK (already erased).");
    }
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

    for (size_t i = 0; i < len; i++)
        buffer[i] = SPI.transfer(0x00);

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

        for (size_t i = 0; i < chunk; i++)
            SPI.transfer(data[written + i]);

        digitalWrite(FLASH_CS_PIN, HIGH);

        // Wait for write to complete
        while (flashIsBusy());

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

    while (flashIsBusy());
    flashWriteDisable();
}

// -------------------- Dump Utilities --------------------

void flashDumpRange(uint32_t addr, size_t len) {
    const size_t chunkSize = 256;
    uint8_t buffer[chunkSize];

    while (len > 0) {
        size_t toRead = min(chunkSize, len);
        flashRead(addr, buffer, toRead);

        for (size_t i = 0; i < toRead; i++)
            Serial.write(buffer[i]);

        addr += toRead;
        len  -= toRead;
    }
}

void flashDumpAll() {
    flashDumpRange(0, FLASH_SIZE_BYTES);
}