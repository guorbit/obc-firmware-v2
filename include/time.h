#ifndef TIME_H
#define TIME_H

#include <Arduino.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

void rtcSetTime(char* isotime);
// Function to read the current date and time from the STM32's RTC.
// Returns a formatted string like: "2025-10-18T 14:23:05Z"
const char* rtcGetTime(void);

// Function to initialize the RTC peripheral
void rtcInit(void);

#ifdef __cplusplus
}
#endif

#endif // TIME_H