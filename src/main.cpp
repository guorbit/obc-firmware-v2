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

    // Initialize flash and metadata
    flashInit();

    Serial.println("\n==== SaveState Test Start ====\n");

    const char msg1[] = "HELLO WORLD";
    const char msg2[] = "SECOND MESSAGE";
    String msg3 = "THIRD MESSAGE (from String)";

    // ---- Save first message (C-string) ----
    uint32_t firstAddr = saveState(msg1, strlen(msg1));
    if (firstAddr == 0) Serial.println("Failed to save first message!");

    // ---- Save second message (C-string) ----
    uint32_t secondAddr = saveState(msg2, strlen(msg2));
    if (secondAddr == 0) Serial.println("Failed to save second message!");

    // ---- Save third message (String type) ----
    uint32_t thirdAddr = saveStateString(msg3);
    if (thirdAddr == 0) Serial.println("Failed to save third message!");

    // ---- Read back messages ----
    uint8_t buffer[128];

    // Read first message
    memset(buffer, 0, sizeof(buffer));
    flashRead(firstAddr, buffer, strlen(msg1));
    buffer[strlen(msg1)] = '\0';
    Serial.print("Read back [1]: ");
    Serial.println((char*)buffer);

    // Read second message
    memset(buffer, 0, sizeof(buffer));
    flashRead(secondAddr, buffer, strlen(msg2));
    buffer[strlen(msg2)] = '\0';
    Serial.print("Read back [2]: ");
    Serial.println((char*)buffer);

    // Read third message
    memset(buffer, 0, sizeof(buffer));
    flashRead(thirdAddr, buffer, msg3.length());
    buffer[msg3.length()] = '\0';
    Serial.print("Read back [3]: ");
    Serial.println((char*)buffer);

    // ---- Print next free address ----
    Serial.print("\nNext free address is now 0x");
    Serial.println(flashGetNextFreeAddr(), HEX);

    Serial.println("\n==== SaveState Test Complete ====\n");
}

void loop() {
    // Simple blink and TMP test
    blink(PD_13);
    Serial.printf("TMP: %i\n", tmp());
    delay(50);
}
