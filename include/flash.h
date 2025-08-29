#ifndef FLASH_H
#define FLASH_H

#include <Arduino.h>
#include <SPI.h>

// Initialize SPI and flash chip
void flashInit();

// Read 'len' bytes from 'addr' into buffer
void flashRead(uint32_t addr, uint8_t* buffer, size_t len);

// Write 'len' bytes from data to 'addr'
void flashWrite(uint32_t addr, const uint8_t* data, size_t len);

#endif // FLASH_H
