#include <Arduino.h>
#include "burnwire.h"

//INITIALISATION
void initBurnwire() {

    // configure pin as output so we can force it to be high or low (off or on)
    pinMode(GPIO_BURNWIRE, OUTPUT);

    // make sure burnwire is off at the start
    digitalWriteFast(GPIO_BURNWIRE, LOW);
}

//SETBURNWIRE FUNCTION
int setBurnwire(bool state) { // giving function input value of 'state', a boolean
    digitalWriteFast(GPIO_BURNWIRE, state); // this lines sets the state of the burnwire to on or off
    return state;
}