#ifndef SAVE_H
#define SAVE_H

#include <Arduino.h>
#include <SPI.h>
#include "flash.h"
#include <string>

// -------------------- Flash Configuration --------------------

// 16 MB total size
#define FLASH_SIZE_BYTES (16 * 1024 * 1024)

// 4 KB reserved for metadata (tally bits)
#define FLASH_METADATA_SIZE 0x1000

// Block size of user data per tally bit (adjustable)
#define FLASH_BLOCK_SIZE 512

// First usable user data address (after metadata region)
#define FLASH_USER_START FLASH_METADATA_SIZE

// -------------------- SaveState Functions --------------------

/**
 * @brief Saves a character array to flash without overwriting existing data.
 * Data is written to the first unused block based on the metadata tally.
 * 
 * @param data  Pointer to character array
 * @param len   Number of bytes to write
 * @return The flash address the data was written to, or 0 on failure.
 */
uint32_t saveState(const char* data, size_t len);

/**
 * @brief Saves a std::string to flash.
 * Converts the string into a char buffer before writing.
 * 
 * @param s  The std::string to save.
 * @return The flash address the data was written to, or 0 on failure.
 */
uint32_t saveStateString(const std::string& s);

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
