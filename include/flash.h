#ifndef FLASH_H
#define FLASH_H

#include <Arduino.h>
#include <SPI.h>

// Flash size constant
#define FLASH_SIZE_BYTES (16 * 1024 * 1024) // 16 MB total size

// -------------------- SPI Flash Functions --------------------

// Initialize SPI and flash chip
void flashInit();

// Read 'len' bytes from 'addr' into buffer
void flashRead(uint32_t addr, uint8_t* buffer, size_t len);

// Write 'len' bytes from data to 'addr'
void flashWrite(uint32_t addr, const uint8_t* data, size_t len);

bool flashIsBusy();

// Dumps a given range (partial dump)
void flashDumpRange(uint32_t addr, size_t len);

// Dumps the entire 16 MB flash contents over serial
// flashDumpAll() is just a wrapper that calls flashDumpRange(0, 16MB)
void flashDumpAll();

#endif // FLASH_H
