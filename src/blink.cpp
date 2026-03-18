#include <Arduino.h>

const long interval = 100;    // base interval
const uint8_t intervalsOn = 1;    // number of intervals to remain on
const uint8_t intervalsTotal = 5; // number of intervals for one on-off cycle

uint8_t currentIntervals = intervalsTotal - 1; // assigned intervalsTotal -1 to start on
unsigned long previousMillis = 0;      // time of the last interval
unsigned long currentMillis = 0;

void blink(PinName led)
{
  currentMillis = millis(); // the current uptime (milliseconds)

  // if one interval has passed, then reset timer and iterate currentIntervals
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    currentIntervals++;
  }

  switch (currentIntervals)
  {
  case intervalsOn: // turn LED off at intervalsOn
    digitalWriteFast(led, LOW);
    break;
  case intervalsTotal: // turn LED on and reset cycle at intervalsTotal
    digitalWriteFast(led, HIGH);
    currentIntervals = 0;
    break;
  }

  // Reset counter if out of bounds
  if (currentIntervals > intervalsTotal) {
    currentIntervals = 0;
  }
}

// Wrapper for blink setup
bool blinkInit() {
  pinMode(PD13, OUTPUT);       // status LED
  return EXIT_SUCCESS;
}

// Wrapper for blink polling
bool blinkPoll() {
  blink(PD_13);
  return EXIT_SUCCESS;
}