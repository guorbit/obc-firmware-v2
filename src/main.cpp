#include <Arduino.h>
#include <blink.h>
#include <tmp.h>
#include "flash.h"  // SPI flash support

void setup() {
    // -------------------- Setup --------------------
    pinMode(PD13, OUTPUT);     // status LED
    Serial.begin(9600);        // initialize serial for debug output
    flashInit();               // initialize SPI flash
}

void loop() {
    // -------------------- Main Loop --------------------
    blink(PD_13);                  // blink status LED
    Serial.printf("TMP: %i\n", tmp());  // print TMP value
    delay(50);
}
