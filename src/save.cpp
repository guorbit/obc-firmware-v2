#include "save.h"
#include "flash.h"
#include <Arduino.h>
#include <string>

// -------------------- Metadata Buffer --------------------
// Optional: can be read byte-by-byte if RAM is tight
static uint8_t metadata_buffer[FLASH_METADATA_SIZE];

// Maximum number of blocks tracked by the metadata tally
#define MAX_METADATA_BITS ((FLASH_SIZE_BYTES - FLASH_USER_START) / FLASH_BLOCK_SIZE)

// -------------------- Metadata Helpers --------------------

uint32_t flashGetUserStart() {
    return FLASH_USER_START;
}

/**
 * Scans the metadata sector to find the first free block (bit = 1)
 */
uint32_t flashGetNextFreeAddr() {
    for (uint32_t byte_idx = 0; byte_idx < FLASH_METADATA_SIZE; ++byte_idx) {
        uint8_t current_byte;
        flashRead(byte_idx, &current_byte, 1);

        if (current_byte == 0x00) {
            continue; // all blocks used in this byte
        }

        for (int bit_pos = 7; bit_pos >= 0; --bit_pos) {
            if ((current_byte >> bit_pos) & 0x01) {
                uint32_t bit_index = (byte_idx * 8) + (7 - bit_pos);

                if (bit_index >= MAX_METADATA_BITS) {
                    Serial.println("Warning: flash full.");
                    return FLASH_SIZE_BYTES;
                }

                return FLASH_USER_START + (bit_index * FLASH_BLOCK_SIZE);
            }
        }
    }

    // All bits used
    Serial.println("Warning: Metadata tally full.");
    return FLASH_SIZE_BYTES;
}

/**
 * Updates the metadata after a successful write
 */
void flashAdvanceNextFreeAddr(uint32_t justWrittenAddr, size_t len) {
    if (justWrittenAddr < FLASH_USER_START) {
        Serial.println("Error: invalid address for metadata update.");
        return;
    }

    uint32_t block_index = (justWrittenAddr - FLASH_USER_START) / FLASH_BLOCK_SIZE;
    uint32_t blocks_to_mark = (len + FLASH_BLOCK_SIZE - 1) / FLASH_BLOCK_SIZE;

    for (uint32_t i = 0; i < blocks_to_mark; ++i) {
        uint32_t current_index = block_index + i;

        if (current_index >= MAX_METADATA_BITS) {
            Serial.println("Metadata tally full during advance. Erasing sector...");
            flashEraseSector(0x000000);
            return;
        }

        uint32_t byte_idx = current_index / 8;
        int bit_pos = 7 - (current_index % 8);

        uint8_t current_byte;
        flashRead(byte_idx, &current_byte, 1);

        uint8_t new_byte = current_byte & ~(1 << bit_pos);

        flashWrite(byte_idx, &new_byte, 1);

        // Wait for completion
        while (flashIsBusy()) {
            // just wait until write completes
        }
    }
}

// -------------------- Save State --------------------

uint32_t saveState(const char* data, size_t len) {
    if (len == 0) {
        Serial.println("Warning: saveState called with empty data");
        return 0;
    }

    uint32_t addr = flashGetNextFreeAddr();

    if (addr + len > FLASH_SIZE_BYTES || addr == FLASH_SIZE_BYTES) {
        Serial.println("Error: Not enough space in flash.");
        return 0;
    }

    if (len > FLASH_BLOCK_SIZE) {
        Serial.println("Error: Data exceeds FLASH_BLOCK_SIZE.");
        return 0;
    }

    flashWrite(addr, reinterpret_cast<const uint8_t*>(data), len);

    while (flashIsBusy()) {
        // waiting for write to complete
    }

    flashAdvanceNextFreeAddr(addr, len);

    Serial.print("saveState: wrote ");
    Serial.print(len);
    Serial.print(" bytes at address 0x");
    Serial.println(addr, HEX);

    return addr;
}

uint32_t saveStateString(const std::string& s) {
    if (s.empty()) {
        Serial.println("Warning: saveStateString called with empty data");
        return 0;
    }

    // Convert std::string → char array
    const char* cstr = s.c_str();

    // Delegate to saveState()
    return saveState(cstr, s.size());
}


