#ifndef SAVE_H
#define SAVE_H

#include <Arduino.h>
#include <SPI.h>

// --- Configuration Constants ---
// Size of user data allocated per metadata bit (e.g., 512 bytes)
// Must be larger than the max expected size of a single saveState call.
#define FLASH_BLOCK_SIZE 512 
// Reserve a 4KB sector for metadata (1 byte = 8 blocks/writes)
#define FLASH_METADATA_SIZE 0x1000 


// -------------------- saveState Functions --------------------
// Saves a character array to flash, appending after previous data.
// Returns the address it was written to, or 0 on failure.
uint32_t saveState(const char* data, size_t len);

// -------------------- Metadata Helpers --------------------

// Reads the "next free" address by scanning the metadata tally.
uint32_t flashGetNextFreeAddr();

// Updates the metadata tally by flipping the current '1' bit to '0'.
// This internally handles sector erasure when the tally is full.
void flashAdvanceNextFreeAddr(size_t len);

// Get the first usable user address (after metadata)
uint32_t flashGetUserStart();

#endif // SAVE_H