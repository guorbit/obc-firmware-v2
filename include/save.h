#ifndef SAVE_H
#define SAVE_H

#include <Arduino.h>
#include <SPI.h>
#include "flash.h"

// -------------------- Configuration --------------------
// All constants now come from flash.h for consistency.
// FLASH_BLOCK_SIZE  = 512 bytes per user write
// FLASH_METADATA_SIZE = 4KB reserved for metadata
// FLASH_USER_START = first usable data address

// -------------------- saveState Functions --------------------

/**
 * @brief Saves a character array to flash without overwriting existing data.
 * Data is written to the first unused block based on the metadata tally.
 * 
 * @param data  Pointer to character array
 * @param len   Number of bytes to write
 * @return The flash address the data was written to, or 0 on failure
 */
uint32_t saveState(const char* data, size_t len);

// -------------------- Metadata Helpers --------------------

/**
 * @brief Reads the metadata tally to find the next free address in flash.
 * @return The next free address for user data.
 */
uint32_t flashGetNextFreeAddr();

/**
 * @brief Updates the metadata tally after a successful write.
 * Flips one or more bits from 1 → 0 to mark blocks as used.
 * 
 * @param justWrittenAddr The starting address of the last write.
 * @param len The number of bytes written.
 */
void flashAdvanceNextFreeAddr(uint32_t justWrittenAddr, size_t len);

/**
 * @brief Returns the first usable flash address after metadata.
 */
uint32_t flashGetUserStart();

#endif // SAVE_H
