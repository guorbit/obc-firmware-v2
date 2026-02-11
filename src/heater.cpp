#include <Arduino.h>
#include "heater.h"

//INITIALISATION
void heaterInit() {

    // configure pin as output so we can force it to be high or low (off or on)
    pinMode(GPIO_HEATER, OUTPUT);

    // make sure heater is off at the start
    digitalWriteFast(GPIO_HEATER, LOW);
}

//SETHEATER FUNCTION
int setHeater(bool state) { // giving function input value of 'state', a boolean
    digitalWriteFast(GPIO_HEATER, state); // this lines sets the state of the heater to on or off
    return state;
}