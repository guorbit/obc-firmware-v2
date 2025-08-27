#include "flash.h"

// Define the Chip Select (CS) pin for your SPI flash
#define FLASH_CS_PIN 10  // <-- replace with actual pin from your schematic

void flashInit() {
    pinMode(FLASH_CS_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH); // CS idle high
    SPI.begin();
}

void flashRead(uint32_t addr, uint8_t* buffer, size_t len) {
    // TODO: implement SPI read command (0x03)
}

void flashWrite(uint32_t addr, const uint8_t* data, size_t len) {
    // TODO: implement SPI write command (0x06 + 0x02)
}
