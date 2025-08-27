#include <Arduino.h>
#include <blink.h>
#include <tmp.h>
#include "flash.h"  // Include the flash feature

void setup() {
    // -------------------- Setup --------------------
    pinMode(PD13, OUTPUT);     // status LED
    Serial.begin(9600);        // initialize serial for debug output
    flashInit();               // initialize SPI flash

    // -------------------- Flash Test --------------------
    const uint8_t testData[5] = { 'H','E','L','L','O' }; // sample data
    uint8_t readBuffer[5] = {0};

    // Write 5 bytes to start of flash (address 0x000000)
    flashWrite(0x000000, testData, 5);

    // Read back the 5 bytes
    flashRead(0x000000, readBuffer, 5);

    // Print read buffer in HEX
    Serial.print("Read buffer: ");
    for (int i = 0; i < 5; i++) {
        Serial.print(readBuffer[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    Serial.println("Flash test complete.");
    // ----------------------------------------------------
}

void loop() {
    // -------------------- Main Loop --------------------
    blink(PD_13);                 // blink status LED
    Serial.printf("TMP: %i\n", tmp());  // print TMP value
    delay(50);
}
