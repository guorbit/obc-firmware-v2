#include <Arduino.h>
#include <blink.h>
#include <tmp.h>
#include <flash.h>
#include <save.h>

void setup() {
    pinMode(PD13, OUTPUT);
    Serial.begin(115200);
    flashInit();

    // -------------------- SaveState Test --------------------
    const char msg1[] = "HELLO WORLD";
    const char msg2[] = "SECOND MESSAGE";

    // Save messages and get addresses
    uint32_t firstAddr  = saveState(msg1, strlen(msg1));
    if (firstAddr == 0) Serial.println("Failed to save first message!");

    uint32_t secondAddr = saveState(msg2, strlen(msg2));
    if (secondAddr == 0) Serial.println("Failed to save second message!");

    // Read back first message
    uint8_t buffer[32] = {0};
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

    // Check final pointer position
    Serial.print("Next free address is now 0x");
    Serial.println(flashGetNextFreeAddr(), HEX);
    Serial.println("SaveState test complete.");
}

void loop() {
    blink(PD_13);
    Serial.printf("TMP: %i\n", tmp());
    delay(50);
}
