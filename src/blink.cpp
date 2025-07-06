#include <Arduino.h>

const long interval = 100; // base interval
const int intervalsOn = 1; // number of intervals to remain on
const int intervalsTotal = 5; // number of intervals for one on-off cycle

int currentIntervals = intervalsTotal; // assigned intervalsTotal to start on
unsigned long previousMillis = 0; // time of the last interval

void blink(PinName led)
{  
  unsigned long currentMillis = millis(); // the current uptime (milliseconds)

  // if one interval has passed, then reset timer and iterate currentIntervals
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    currentIntervals++;
  }

  switch (currentIntervals) {
    case intervalsOn: // turn LED off at intervalsOn
      digitalWriteFast(led, LOW);
      break;
    case intervalsTotal: // turn LED on and reset cycle at intervalsTotal
      digitalWriteFast(led, HIGH);
      currentIntervals = 0;
      break;
  }
}