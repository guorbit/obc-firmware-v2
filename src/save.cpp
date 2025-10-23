#include "save.h"
#include "flash.h"
#include <Arduino.h>

// --- Constants (must match save.h) ---
#define FLASH_BLOCK_SIZE    512 
#define FLASH_METADATA_SIZE 0x1000 
#define FLASH_USER_START    FLASH_METADATA_SIZE 
#define FLASH_SIZE_BYTES    (16 * 1024 * 1024) 
#define MAX_METADATA_BITS   ((FLASH_SIZE_BYTES - FLASH_METADATA_SIZE) / FLASH_BLOCK_SIZE)

// NOTE: This array is large, ensure your microcontroller has enough RAM for this buffer.
// For a 4KB metadata sector (0x1000), this buffer is 4096 bytes.
// If RAM is too small, this should be read/written byte-by-byte instead.
uint8_t metadata_buffer[FLASH_METADATA_SIZE]; 

// Forward declaration from flash.cpp
extern void flashEraseSector(uint32_t addr); 

// -------------------- Metadata Pointer --------------------

uint32_t flashGetUserStart() {
    return FLASH_USER_START;
}

// Scans the metadata tally to find the next free address.
uint32_t flashGetNextFreeAddr() {
    // 1. Read the entire metadata sector (4KB)
    flashRead(0x000000, metadata_buffer, FLASH_METADATA_SIZE);

    // 2. Scan the buffer for the first bit set to '1' (free)
    for (uint32_t byte_idx = 0; byte_idx < FLASH_METADATA_SIZE; ++byte_idx) {
        uint8_t current_byte = metadata_buffer[byte_idx];
        
        // Skip this byte if all bits are 0 (fully used)
        if (current_byte == 0x00) continue; 

        // Find the first '1' bit within this byte (bit 7 is MSB)
        for (int bit_pos = 7; bit_pos >= 0; --bit_pos) {
            if ((current_byte >> bit_pos) & 0x01) {
                // Found the first '1' bit! Calculate the corresponding index.
                uint32_t bit_index = (byte_idx * 8) + (7 - bit_pos);
                
                // Check against chip size limit
                if (bit_index >= MAX_METADATA_BITS) {
                    Serial.println("Warning: Metadata pointer indicates out of bounds, resetting.");
                    // This error indicates the entire chip is full.
                    return FLASH_SIZE_BYTES; 
                }

                // Calculate the next free user address
                uint32_t next_free_addr = FLASH_USER_START + (bit_index * FLASH_BLOCK_SIZE);
                return next_free_addr;
            }
        }
    }

    // If we reach here, the entire metadata block is full (all 0s).
    Serial.println("Warning: Metadata tally full. Chip is logically full or needs reset.");
    return FLASH_SIZE_BYTES; // Return maximum address to signal no space.
}

// Updates the metadata tally by flipping the current '1' bit to '0'.
void flashAdvanceNextFreeAddr(size_t len) {
    
    // 1. Recalculate the index of the block *just* written
    uint32_t current_addr = flashGetNextFreeAddr();
    if (current_addr == FLASH_USER_START) {
        // Special case: we must be at the very start of the chip.
        // The first block will be index 0. We'll proceed to write bit 0 to 0.
    } else if (current_addr > FLASH_USER_START) {
        // Find the index of the block that was *just* written.
        current_addr -= FLASH_BLOCK_SIZE; // Roll back one block size
    } else {
        Serial.println("Error: Advance called when chip is full.");
        return; 
    }

    uint32_t bit_index = (current_addr - FLASH_USER_START) / FLASH_BLOCK_SIZE;

    // Check if the current write is too large for one block
    if (len > FLASH_BLOCK_SIZE) {
        Serial.println("Error: Data size exceeds FLASH_BLOCK_SIZE. Will corrupt metadata.");
        // For simplicity, we only advance one block, but a real fix would require
        // writing multiple metadata bits for one large data chunk.
    }

    // 2. Check if we need to Erase (Wear-Leveling)
    if (bit_index >= MAX_METADATA_BITS) {
        // The last block has been written. Erase the metadata sector and reset.
        Serial.println("Metadata tally FULL! Erasing metadata sector (0x000000) and wrapping.");
        flashEraseSector(0x000000); 
        // After erase, all bits are '1', so the next 'flashGetNextFreeAddr' will correctly
        // return FLASH_USER_START again, and the next write will target bit 0.
        return;
    }

    // 3. Program the single bit from '1' to '0'
    uint32_t byte_idx = bit_index / 8;
    int bit_pos = 7 - (bit_index % 8); // 7 is MSB

    // Read the *current* byte from the buffer (it was loaded in flashGetNextFreeAddr)
    uint8_t original_byte = metadata_buffer[byte_idx];
    
    // Calculate the new byte value by clearing the target bit (1 -> 0)
    uint8_t new_byte = original_byte & ~(1 << bit_pos);

    // Only write 1 byte to the flash chip.
    uint32_t flash_byte_addr = 0x000000 + byte_idx;
    flashWrite(flash_byte_addr, &new_byte, 1);
    
    Serial.print("Advanced tally: Bit ");
    Serial.print(bit_index);
    Serial.print(" (Addr 0x");
    Serial.print(flash_byte_addr, HEX);
    Serial.println(")");
}

// -------------------- Save State --------------------
uint32_t saveState(const char* data, size_t len) {
    if (len == 0) {
        Serial.println("Warning: saveState called with empty data");
        return 0;  // indicate failure
    }

    uint32_t addr = flashGetNextFreeAddr();

    if (addr + len > FLASH_SIZE_BYTES || addr == FLASH_SIZE_BYTES) {
        Serial.println("Error: Not enough space in flash to save state or chip is full.");
        return 0;  // failure
    }
    
    // Check if the data size fits within the allocated block
    if (len > FLASH_BLOCK_SIZE) {
        Serial.println("Error: Data size exceeds FLASH_BLOCK_SIZE. Aborting.");
        return 0; 
    }

    // Write the data
    flashWrite(addr, reinterpret_cast<const uint8_t*>(data), len);

    // Advance the pointer (this flips the metadata bit from 1 to 0)
    flashAdvanceNextFreeAddr(len);

    Serial.print("saveState: wrote ");
    Serial.print(len);
    Serial.print(" bytes at address 0x");
    Serial.println(addr, HEX);

    return addr;
}