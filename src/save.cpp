#include "save.h"
#include "flash.h"
#include <Arduino.h>

#define FLASH_METADATA_SIZE 0x100  
#define FLASH_POINTER_ADDR  0x000000  
#define FLASH_USER_START    0x000004  
#define FLASH_SIZE_BYTES    (16 * 1024 * 1024)  // must match flash.cpp

// -------------------- Metadata Pointer --------------------
uint32_t flashGetNextFreeAddr() {
    uint8_t buf[4];
    flashRead(FLASH_POINTER_ADDR, buf, 4);

    uint32_t addr = ((uint32_t)buf[0] << 24) |
                    ((uint32_t)buf[1] << 16) |
                    ((uint32_t)buf[2] << 8)  |
                    ((uint32_t)buf[3]);

    if (addr < FLASH_USER_START || addr >= FLASH_SIZE_BYTES) {
        Serial.println("Warning: flash pointer invalid, resetting to FLASH_USER_START");
        addr = FLASH_USER_START;
        flashSetNextFreeAddr(addr);
    }

    return addr;
}

void flashSetNextFreeAddr(uint32_t addr) {
    if (addr < FLASH_USER_START || addr >= FLASH_SIZE_BYTES) {
        Serial.println("Error: Attempt to set flash pointer outside valid range");
        return;
    }

    uint8_t buf[4];
    buf[0] = (addr >> 24) & 0xFF;
    buf[1] = (addr >> 16) & 0xFF;
    buf[2] = (addr >> 8) & 0xFF;
    buf[3] = addr & 0xFF;

    flashWrite(FLASH_POINTER_ADDR, buf, 4);
}

uint32_t flashGetUserStart() {
    return FLASH_USER_START;
}

// -------------------- Save State --------------------
uint32_t saveState(const char* data, size_t len) {
    if (len == 0) {
        Serial.println("Warning: saveState called with empty data");
        return 0;  // indicate failure
    }

    uint32_t addr = flashGetNextFreeAddr();

    if (addr + len >= FLASH_SIZE_BYTES) {
        Serial.println("Error: Not enough space in flash to save state");
        return 0;  // failure
    }

    flashWrite(addr, reinterpret_cast<const uint8_t*>(data), len);
    flashSetNextFreeAddr(addr + len);

    Serial.print("saveState: wrote ");
    Serial.print(len);
    Serial.print(" bytes at address 0x");
    Serial.println(addr, HEX);

    return addr;
}
