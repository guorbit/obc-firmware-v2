#include <Arduino.h>
#include <blink.h>
#include <tmp.h>
#include "flash.h"  // SPI flash support

void setup() {
    // -------------------- Setup --------------------
    pinMode(PD13, OUTPUT);     // status LED
    Serial.begin(921600);        // initialize serial for debug output

    // TODO Need to mess around with the baud rate to optimise the speed vs reliability

    flashInit();               // initialize SPI flash

    // Example: Dump first 1 KB
    flashDumpRange(0, 1024);
}

void loop() {
    // -------------------- Main Loop --------------------
    blink(PD_13);                  // blink status LED
    Serial.printf("TMP: %i\n", tmp());  // print TMP value
    delay(50);
}
