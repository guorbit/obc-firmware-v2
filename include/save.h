#ifndef SAVE_H
#define SAVE_H

#include <Arduino.h>
#include <SPI.h>
#include "flash.h"
#include <string>

/**
 * @brief Saves a character array to flash without overwriting existing data.
 * 
 * Reads a flash metadata tally (which is stored in the first sections of the
 * flash chip) and selects the first unused block based on the metadata tally
 * to avoid overwriting existing data. Checks that the data to be written 
 * will fit in the remaining unused space. Writes data to address, awaits 
 * flash write confirmation, and updates metadata tally.
 *  
 * @param[in] data  Pointer to character array.
 * @param[in] len   Number of bytes to write. Must be greater than zero and no
 * larger than FLASH_BLOCK_SIZE. 
 * @return The flash address the data was written to.
 * @retval 0 The data was not stored (error).  
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
