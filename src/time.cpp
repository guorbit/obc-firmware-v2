#include "time.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"  // Include RTC-specific types/functions
#include <cstdio>
#include <cstring>
#include <Arduino.h>
#include <string.h>

RTC_HandleTypeDef hrtc;  // Define the RTC handle globally

// -------------------- RTC Setup -------------------
void rtcSetTime(String isotime){ // Initialise RTC with a specific time and date   

    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    if (isotime.length() < 19) {
        // If no valid time string is provided, use a default time
        isotime = "2026-01-01T00:00:00Z"; 
    }

    int year   = isotime.substring(0, 4).toInt();
    int yearShort = year - 2000; // RTC uses 2-digit year
    int month  = isotime.substring(5, 7).toInt();
    int day    = isotime.substring(8, 10).toInt();
    int hour   = isotime.substring(11, 13).toInt();
    int minute = isotime.substring(14, 16).toInt();
    int second = isotime.substring(17, 19).toInt();

    char buffer[25];
    sprintf(buffer, "Setting RTC to: %04d-%02d-%02dT%02d:%02d:%02dZ\r\n", year, month, day, hour, minute, second);
    Serial.print(buffer);

    sTime.Hours = hour;
    sTime.Minutes = minute;
    sTime.Seconds = second;
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

    sDate.Year = yearShort; // RTC year is offset from 2000
    sDate.Month = month;
    sDate.Date = day;
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

void errorHandler() 
{
    // TODO: Implement error handling (e.g., blink an LED, log the error, etc.)
}

// ---------------- RTC Initialisation Function ------------------
extern "C" void rtcInit()
{
    // Enable Power Clock and Backup Access
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    // Enable LSE Oscillator
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;        
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE; // No PLL update
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        errorHandler();
    }

    // Select LSE as RTC Clock Source
    RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
    PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
    {
        errorHandler();
    }

    // Enable RTC Clock
    __HAL_RCC_RTC_ENABLE();

    // Configure RTC
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if (HAL_RTC_Init(&hrtc) != HAL_OK)
    {
        errorHandler();
    }

    // Check if RTC is already set by reading a backup register
    if (HAL_RTCEx_BKUPRead(&hrtc, RTC_BKP_DR1) != 0x32F2)
    {
        // First time: set time and mark backup register
        rtcSetTime(String("2026-01-01T00:00:00Z")); // Set default time
        HAL_RTCEx_BKUPWrite(&hrtc, RTC_BKP_DR1, 0x32F2);
        __HAL_RCC_CLEAR_RESET_FLAGS(); // Clear reset flags
    }
}

// -------------------- Get Time --------------------
const char* rtcGetTime()
{
    static char buffer[32];  // Holds formatted time text

    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    // Read current time and date from the STM32's internal RTC
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    // Format the values into a readable string: "YYYY-MM-DD HH:MM:SS"
    snprintf(buffer, sizeof(buffer),
             "%04d-%02d-%02dT%02d:%02d:%02dZ\r\n",
             sDate.Year + 2000, sDate.Month, sDate.Date,
             sTime.Hours, sTime.Minutes, sTime.Seconds);

    // Return the formatted string (stored in static memory)
    return buffer;
}