#ifndef FLASH_H
#define FLASH_H

#include <Arduino.h>
#include <SPI.h>

// -------------------- Flash Configuration --------------------

// 16 MB total size
#define FLASH_SIZE_BYTES (16 * 1024 * 1024)

// 4 KB reserved for metadata (tally bits)
#define FLASH_METADATA_SIZE 0x1000

// Block size of user data per tally bit (adjustable)
#define FLASH_BLOCK_SIZE 512

// First usable user data address (after metadata region)
#define FLASH_USER_START FLASH_METADATA_SIZE

// -------------------- SPI Flash Functions --------------------

// Initialize SPI and ensure metadata region is ready
void flashInit();

// Read 'len' bytes from 'addr' into buffer
void flashRead(uint32_t addr, uint8_t* buffer, size_t len);

// Write 'len' bytes from data to 'addr'
void flashWrite(uint32_t addr, const uint8_t* data, size_t len);

// Check if flash is busy performing an operation
bool flashIsBusy();

// Dump a given range over Serial (for debugging)
void flashDumpRange(uint32_t addr, size_t len);

// Dump the entire 16MB chip (be cautious, slow!)
void flashDumpAll();

// Erase a 4KB sector that contains 'addr'
void flashEraseSector(uint32_t addr);

#endif // FLASH_H
