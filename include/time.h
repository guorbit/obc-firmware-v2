#ifndef TIME_H
#define TIME_H

#ifdef __cplusplus
extern "C" {
#endif

// Function to read the current date and time from the STM32's RTC.
// Returns a formatted string like: "2025-10-18 14:23:05"
const char* getTime(void);

#ifdef __cplusplus
}
#endif

#endif // TIME_H