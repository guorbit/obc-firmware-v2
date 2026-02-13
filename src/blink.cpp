#include <Arduino.h>

const long interval = 100;    // base interval
const uint8_t intervalsOn = 1;    // number of intervals to remain on
const uint8_t intervalsTotal = 5; // number of intervals for one on-off cycle

uint8_t currentIntervals = intervalsTotal; // assigned intervalsTotal to start on
unsigned long previousMillis = 0;      // time of the last interval
unsigned long currentMillis = 0;

void blink(PinName led) {
  currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    currentIntervals++;

    // Use modulo to keep currentIntervals looping between 0 and 4
    uint8_t cyclePosition = currentIntervals % intervalsTotal;

    // If position is less than our "On" count, stay high, else low
    digitalWriteFast(led, (cyclePosition < intervalsOn) ? HIGH : LOW);
  }
}