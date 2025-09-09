#include <Arduino.h>
#include <blink.h>
#include <tmp.h>
#include "flash.h"  // SPI flash support

void setup() {
    // -------------------- Setup --------------------
    pinMode(PD13, OUTPUT);     // status LED
    Serial.begin(115200);      // initialize serial for debug output

    flashInit();               // initialize SPI flash

    // -------------------- Setup --------------------
    pinMode(PD13, OUTPUT);     // status LED
    Serial.begin(115200);      // initialize serial for debug output

    flashInit();               // initialize SPI flash

    // -------------------- SaveState Test --------------------
    const char msg1[] = "HELLO WORLD";
    const char msg2[] = "SECOND MESSAGE";

    saveState(msg1, strlen(msg1));
    saveState(msg2, strlen(msg2));

    // ---- Read back first message ----
    uint8_t buffer[32] = {0};
    uint32_t firstAddr = flashGetUserStart();
    flashRead(firstAddr, buffer, strlen(msg1));

    Serial.print("Read back first message: ");
    for (size_t i = 0; i < strlen(msg1); i++) {
        Serial.print((char)buffer[i]);
    }
    Serial.println();

    // ---- Read back second message ----
    uint32_t secondAddr = firstAddr + strlen(msg1);
    memset(buffer, 0, sizeof(buffer));
    flashRead(secondAddr, buffer, strlen(msg2));

    Serial.print("Read back second message: ");
    for (size_t i = 0; i < strlen(msg2); i++) {
        Serial.print((char)buffer[i]);
    }
    Serial.println();

    // ---- Check final pointer position ----
    Serial.print("Next free address is now 0x");
    Serial.println(flashGetNextFreeAddr(), HEX);
    Serial.println("SaveState test complete.");
    // ---------------------------------------------------------
}

void loop() {
    // -------------------- Main Loop --------------------
    blink(PD_13);                  // blink status LED
    Serial.printf("TMP: %i\n", tmp());  // print TMP value
    delay(50);
}
