#ifndef FLASH_H
#define FLASH_H

#include <Arduino.h>
#include <SPI.h>

// -------------------- SPI Flash Functions --------------------

// Initialize SPI and flash chip
void flashInit();

// Read 'len' bytes from 'addr' into buffer
void flashRead(uint32_t addr, uint8_t* buffer, size_t len);

// Write 'len' bytes from data to 'addr'
void flashWrite(uint32_t addr, const uint8_t* data, size_t len);

// Dumps a given range (partial dump)
void flashDumpRange(uint32_t addr, size_t len);

// Dumps the entire 16 MB flash contents over serial
// flashDumpAll() is just a wrapper that calls flashDumpRange(0, 16MB)
void flashDumpAll();

// -------------------- saveState Functions --------------------

// Saves a character array to flash, appending after previous data
void saveState(const char* data);

// -------------------- Metadata Helpers --------------------

// Reads the "next free" address from flash (0x000000–0x000003)
uint32_t flashGetNextFreeAddr();

// Writes the "next free" address to flash (0x000000–0x000003)
void flashSetNextFreeAddr(uint32_t addr);

// -------------------- Memory Map --------------------
// 0x000000 – 0x0000FF : Reserved for metadata (pointer, future info)
// 0x000004 – 0xFFFFFF : User data (linear writes / saveState)

#endif // FLASH_H
