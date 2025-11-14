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

    Serial.println("Setting RTC Time and Date.");
    Serial.println("Please enter the format: YYYY-MM-DD HH:MM:SS ");
    while (Serial.available() == 0) {
        // wait for input
    }

    String dateInput = Serial.readStringUntil('\n');    //read input until enter
    dateInput.trim();                                  //remove any leading/trailing whitespace
  
    // basic format validation
    if (dateInput.length() != 19 ||
        dateInput.charAt(4) != '-' ||
        dateInput.charAt(7) != '-' ||
        dateInput.charAt(10) != ' ' ||
        dateInput.charAt(13) != ':' ||
        dateInput.charAt(16) != ':') {

        Serial.println("Invalid format. Use: YYYY-MM-DD HH:MM:SS");
        return;
    }
  
    // extract values
    int year    = dateInput.substring(0, 4).toInt();
    int yearShort = year - 2000; // RTC uses 2-digit year
    int month   = dateInput.substring(5, 7).toInt();
    int day     = dateInput.substring(8, 10).toInt();
    int hour    = dateInput.substring(11, 13).toInt(); 
    int minute  = dateInput.substring(14, 16).toInt();
    int second  = dateInput.substring(17, 19).toInt();

    // validate ranges
    if (month < 1 || month > 12 || day < 1 || day > 31) {   
        Serial.println("Invalid date values. Please ensure month is 1-12 and day is 1-31.");
        return;
    }

    if (hour < 0 || hour > 23 || minute < 0 || minute > 59 || second < 0 || second > 59) {   
        Serial.println("Invalid time values. Please ensure hours are 0-23, minutes and seconds are 0-59.");
        return;
    }

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