#include <Arduino.h>
#include "blink.h"
constexpr uint_fast32_t interval = 440;    // base interval
constexpr int intervalsOn = 7;    // number of intervals to remain on

int currentIntervals = intervalsTotal; // assigned intervalsTotal to start on
unsigned long previousMillis = 0;      // time of the last interval

void blink(PinName led){

  unsigned long currentMillis = millis(); // the current uptime (milliseconds)

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
}