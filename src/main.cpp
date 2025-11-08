#include <Arduino.h>
#include <blink.h>
#include <tmp.h>
#include "flash.h"
#include "save.h"

void setup() {
    pinMode(PD_13, OUTPUT);
    Serial.begin(115200);

    // Wait for Serial to initialize
    delay(2000);

    // Initialize flash and metadata sector
    flashInit();

    Serial.println("---- SaveState Test ----");

    const char msg1[] = "HELLO WORLD";
    const char msg2[] = "SECOND MESSAGE";

    // Save first message
    uint32_t firstAddr  = saveState(msg1, strlen(msg1));
    if (firstAddr == 0) Serial.println("Failed to save first message!");

    // Save second message
    uint32_t secondAddr = saveState(msg2, strlen(msg2));
    if (secondAddr == 0) Serial.println("Failed to save second message!");

    // Buffer for reading back messages
    uint8_t buffer[64];

    // Read back first message
    memset(buffer, 0, sizeof(buffer));
    flashRead(firstAddr, buffer, strlen(msg1));
    buffer[strlen(msg1)] = '\0';
    Serial.print("Read back first message: ");
    Serial.println((char*)buffer);

    // Read back second message
    memset(buffer, 0, sizeof(buffer));
    flashRead(secondAddr, buffer, strlen(msg2));
    buffer[strlen(msg2)] = '\0';
    Serial.print("Read back second message: ");
    Serial.println((char*)buffer);

    // Print next free address
    Serial.print("Next free address is now 0x");
    Serial.println(flashGetNextFreeAddr(), HEX);

    Serial.println("---- SaveState Test Complete ----");
}

void loop() {
    // Simple blink and TMP test
    blink(PD_13);
    Serial.printf("TMP: %i\n", tmp());
    delay(50);
}
