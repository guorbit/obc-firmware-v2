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
}

void loop() {
    // Simple blink and TMP test
    blink(PD_13);
    Serial.printf("TMP: %i\n", tmp());
    delay(50);
}
