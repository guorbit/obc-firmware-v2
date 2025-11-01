#ifndef BLINK_H
#define BLINK_H
#include <Arduino.h>
// accepts a pin name, blinks led and is non-blocking
// the pin must already be initialised as OUTPUT
void blink(PinName led);

#endif