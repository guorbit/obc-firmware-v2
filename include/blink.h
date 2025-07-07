#ifndef BLINK_H
#define BLINK_H

// accepts a pin name, blinks led and is non-blocking
// the pin must already be initialised as OUTPUT


extern 
    void blink(PinName led);
extern unsigned long previousMillis;

extern int currentIntervals; // assigned intervalsTotal to start on
constexpr int intervalsTotal=10; // number of intervals for one on-off cycle
 
#endif