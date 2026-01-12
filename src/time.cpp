#include "time.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"  // Include RTC-specific types/functions
#include <cstdio>
#include <cstring>
#include <Arduino.h>

RTC_HandleTypeDef hrtc;  // Define the RTC handle globally

// -------------------- RTC Setup -------------------
void rtcSetTime(){ // Initialise RTC with a specific time and date   
    
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};
  
    // extract values
    u_int8_t year    = 2026;
    u_int8_t yearShort = year - 2000; // RTC uses 2-digit year
    u_int8_t month   = 01;
    u_int8_t day     = 01;
    u_int8_t hour    = 00;
    u_int8_t minute  = 00;
    u_int8_t second  = 00;

    // print formatted time on one line
    Serial.println("RTC time set.");
    Serial.println("Time set to: ");
    char buffer[25];
    sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02dZ", year, month, day, hour, minute, second);
    Serial.println(buffer);

    sTime.Hours = hour;
    sTime.Minutes = minute;
    sTime.Seconds = second;
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

    sDate.Month = month;
    sDate.Date = day;
    sDate.Year = yearShort; 
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
        rtcSetTime();
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