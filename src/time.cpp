#include "time.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"  // Include RTC-specific types/functions
#include "stm32f4xx_hal_uart.h" // Include UART-specific types/functions
#include <cstdio>
#include <cstring>

RTC_HandleTypeDef hrtc;  // define the RTC handle globally
UART_HandleTypeDef huart2; // UART handle for serial communication

void Error_Handler() 
{
    while(1) {}
}

extern "C" void MX_RTC_Init()
{
    __HAL_RCC_RTC_ENABLE();

    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
    hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;

    if (HAL_RTC_Init(&hrtc) != HAL_OK)
    {
        // Initialization Error
        Error_Handler();
    }
}

const char* getTime()
{
    static char buffer[32];  // Holds formatted time text

    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    // Read current time and date from the STM32's internal RTC
    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    // Format the values into a readable string: "YYYY-MM-DD HH:MM:SS"
    snprintf(buffer, sizeof(buffer),
             "20%02d-%02d-%02d %02d:%02d:%02d\r\n",
             sDate.Year + 2000, sDate.Month, sDate.Date, // + 2000 as sDate.Year is 25
             sTime.Hours, sTime.Minutes, sTime.Seconds);

    // Return the formatted string (stored in static memory)
    return buffer;
}

// Transmit time over UART for debugging
//HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY); 
//HAL_Delay(1000); // Small delay to ensure transmission completes