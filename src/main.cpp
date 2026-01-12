#include <Arduino.h>
#include "blink.h"
#include "tmp.h"
#include "flash.h"  // SPI flash support
#include "recovery.h"
#include "watchdog.hpp"
#include "time.h"   // RTC support

void setup() {
    // -------------------- Setup --------------------
    pinMode(PD13, OUTPUT);       // status LED
    Serial.begin(115200);        // initialize serial for debug output

    flashInit();                 // initialize SPI flash
    rtcInit();                   // initialize RTC

    pinMode(PB2, INPUT);         // recovery mode pin
    if (digitalRead(PB2) == HIGH) {
        recovery();              // enter recovery mode if pin is high
    }

    iwdg::init_watchdog();
}

void loop() {
    // -------------------- Main Loop --------------------
    blink(PD_13);                     // blink status LED
    Serial.printf("TMP: %i\n", tmp());  // print TMP value

    // Optional: print RTC time periodically
    rtcSetTime(); 

    static unsigned long lastPrint = 0;
    if (millis() - lastPrint >= 1000) {
        Serial.printf("RTC Time: %s\n", rtcGetTime());
        lastPrint = millis();
    }

    delay(50);
    iwdg::pet_watch_dog();
}