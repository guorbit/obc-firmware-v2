#ifndef TMP_H
#define TMP_H

// include Arduino.h for int8_t definition, 
// used to avoid wasting RAM with an oversised integer
#include <Arduino.h>

// returns the temperature from the onboard TMP36 sensor 
int tmp();

#endif