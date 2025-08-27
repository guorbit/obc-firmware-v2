#include "flash.h"

// Define the Chip Select (CS) pin for your SPI flash
#define FLASH_CS_PIN PA4

void flashInit() {
    pinMode(FLASH_CS_PIN, OUTPUT);
    digitalWrite(FLASH_CS_PIN, HIGH); // CS idle high
    SPI.begin(); // Uses SCK = PA5, MISO = PA6, MOSI = PA7 by default
}

// ------------------------ Stub implementations ------------------------

// Fill the buffer with dummy data for testing
void flashRead(uint32_t addr, uint8_t* buffer, size_t len) {
    for (size_t i = 0; i < len; i++) {
        buffer[i] = i; // dummy data
    }
}

// Print the data being "written" for testing
void flashWrite(uint32_t addr, const uint8_t* data, size_t len) {
    Serial.print("Simulated write to address 0x");
    Serial.print(addr, HEX);
    Serial.print(": ");
    for (size_t i = 0; i < len; i++) {
        Serial.print(data[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
}
