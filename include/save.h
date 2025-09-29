#ifndef SAVE_H
#define SAVE_H

#include <Arduino.h>
#include <SPI.h>

// -------------------- saveState Functions --------------------
// Saves a character array to flash, appending after previous data.
// Returns the address it was written to, or 0 on failure.
uint32_t saveState(const char* data, size_t len);

// -------------------- Metadata Helpers --------------------
// Get the first usable user address (after metadata)
uint32_t flashGetUserStart();

// Reads the "next free" address from flash (0x000000–0x000003)
uint32_t flashGetNextFreeAddr();

// Writes the "next free" address to flash (0x000000–0x000003)
void flashSetNextFreeAddr(uint32_t addr);

#endif // SAVE_H
