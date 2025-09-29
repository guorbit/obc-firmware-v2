#include "save.h"
#include "flash.h"

// Metadata layout
#define FLASH_METADATA_SIZE 0x100  
#define FLASH_POINTER_ADDR  0x000000  
#define FLASH_USER_START    0x000004  

uint32_t flashGetUserStart() {
    return FLASH_USER_START;
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

void saveState(const char* data, size_t len) {
    if (len == 0) {
        Serial.println("Warning: saveState called with empty data");
        return;
    }

    // Step 1: Get current free address
    uint32_t addr = flashGetNextFreeAddr();

    // Step 2: Check for overflow
    if (addr + len >= FLASH_SIZE_BYTES) {
        Serial.println("Error: Not enough space in flash to save state");
        return;
    }

    // Step 3: Write data at free address
    flashWrite(addr, reinterpret_cast<const uint8_t*>(data), len);

    // Step 4: Update pointer for next free address
    flashSetNextFreeAddr(addr + len);

    Serial.print("saveState: wrote ");
    Serial.print(len);
    Serial.print(" bytes at address 0x");
    Serial.println(addr, HEX);
}



