#ifndef __MAIN_HEADER__ // Added Guard clause

#define __MAIN_HEADER__ // Guard Preprocessor Macro definition

#include <Arduino.h>    // Arduino IDE defintions
#include "blink.h"      // blinking light header
#include "watchdog.hpp" // Watchdog header

#ifdef LED_PIN
#undef LED_PIN // If LED_PIN is not defined, define it
#endif

#define LED_PIN PD_13              // Define LED_PIN as PD_13
#define LED_PINNAME PinName::PD_13 // Define LED_PINNAME as PD_13

#endif