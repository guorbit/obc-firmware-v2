#include <Arduino.h>
#include <blink.h>
#include <tmp.h>
#include "flash.h"  // include the flash feature

void setup() {
    // this code runs once at startup
    pinMode(PD13, OUTPUT);
    Serial.begin(9600);

    // -------------------- flash test --------------------
    flashInit(); // initialize flash (stub)
    
    // test write
    const char testData[5] = {'H','E','L','L','O'};
    flashWrite(0x000000, (uint8_t*)testData, 5);

    // test read
    uint8_t buf[5];
    flashRead(0x000000, buf, 5);

    // print read buffer
    Serial.print("Read buffer: ");
    for (int i = 0; i < 5; i++) {
        Serial.print(buf[i], HEX);
        Serial.print(" ");
    }
    Serial.println();
    // ----------------------------------------------------
}

void loop() {
    // this code loops forever
    blink(PD_13);
    Serial.printf("TMP: %i\n", tmp());
    delay(50);
}
