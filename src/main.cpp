#include "blink.h"
#include "eps.h"
#include "flash.h" // SPI flash support
#include "recovery.h"
#include "save.h"
#include "time.h" // RTC support
#include "tmp.h"
#include "watchdog.hpp"
#include <Arduino.h>
#include <Wire.h>

char mainBuffer[256] = {0}; // buffer to hold state data

void setup() {
  // -------------------- Setup --------------------
  pinMode(PD13, OUTPUT); // status LED
  Serial.begin(115200);  // initialize serial for debug output

  flashInit(); // initialize SPI flash
  rtcInit();   // initialize RTC
  initEPS();   // initialize EPS

  pinMode(PB2, INPUT); // recovery mode pin
  if (digitalRead(PB2) == HIGH) {
    recovery(); // enter recovery mode if pin is high
  }

  // iwdg::init_watchdog();
}

void loop() {
  mainBuffer[0] = '\0'; // clear buffer before use

  // -------------------- Main Loop --------------------
  blink(PD_13);                      // blink status LED
  Serial.printf("TMP: %i\n", tmp()); // print TMP value

  // Optional: print RTC time periodically
  static unsigned long lastPrint = 0;
  if (millis() - lastPrint >= 1000) {
    Serial.printf("RTC Time: %s\n", rtcGetTime());

    sprintf(mainBuffer, rtcGetTime()); // store RTC time in buffer
    sprintf(mainBuffer + strlen(mainBuffer), " | TMP: %i",
            tmp()); // append TMP value to buffer
    sprintf(mainBuffer + strlen(mainBuffer), " | EPS: %s",
            readEPS()); // append EPS readings to buffer

    saveState(mainBuffer, strlen(mainBuffer)); // save state to flash
    Serial.printf("Saved state: %s\n", mainBuffer);

    lastPrint = millis();
  }

  delay(50);
  iwdg::pet_watch_dog();
}