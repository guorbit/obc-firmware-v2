#include "time.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_rtc.h"  // Include RTC-specific types/functions
#include "stm32f4xx_hal_uart.h" // Include UART-specific types/functions
#include <cstdio>
#include <cstring>

RTC_HandleTypeDef hrtc;  // Define the RTC handle globally
UART_HandleTypeDef huart2; // UART handle for serial communication

// --------- RTC Setup and Time Retrieval Functions ---------
void rtcSetTime(void) // Initialise RTC with a specific time and date
{
    RTC_TimeTypeDef sTime = {0};
    RTC_DateTypeDef sDate = {0};

    // Set time manually to 11:01:01
    sTime.Hours = 11;
    sTime.Minutes = 01;
    sTime.Seconds = 01;
    HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);

    sDate.WeekDay = RTC_WEEKDAY_SATURDAY;
    sDate.Month = RTC_MONTH_NOVEMBER;
    sDate.Date = 01;
    sDate.Year = 25; // for 2025
    HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
}

void errorHandler() 
{
    //while(1) {}
}

// -------- GPIO Initialisation Functions ---------
extern "C" void MX_GPIO_Init(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    // Add any LED or button pins here if needed
}

// ------- UART2 Initialisation Function ---------
extern "C" void MX_USART2_UART_Init(void)
{
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Configure PA2 and PA3 for USART2 TX and RX
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;

    if (HAL_UART_Init(&huart2) != HAL_OK)
        errorHandler();
}

// --------- RTC Initialisation Function ---------
extern "C" void MX_RTC_Init()
{
    // Enable Power Clock and Backup Access
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    //Enable LSE Oscillator
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_OscInitStruct.LSEState = RCC_LSE_ON;        
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE; // No PLL update
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        errorHandler();
    }

    //Select LSE as RTC Clock Source
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
        // Initialization Error
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

    // Verify LSE is ready
    if (__HAL_RCC_GET_FLAG(RCC_FLAG_LSERDY)) 
    {
        const char* msg = "LSE ready and stable.\r\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }
    else
    {
        const char* msg = "LSE failed to start!\r\n";
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }
}

// --------- Get Time ---------
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
             "%04d-%02d-%02d %02d:%02d:%02d\r\n",
             sDate.Year + 2000, sDate.Month, sDate.Date,
             sTime.Hours, sTime.Minutes, sTime.Seconds);

    // Return the formatted string (stored in static memory)
    return buffer;
}

// --------- System Clock Configuration Function ---------
extern "C" void clockConfig(void)
    {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    // Enable power control clock
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    // Configure the main internal oscillator (HSI) and PLL
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
    RCC_OscInitStruct.PLL.PLLQ = 7;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        errorHandler();
    }

    // Initialize CPU, AHB, and APB bus clocks
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
                                  RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
    {
        errorHandler();
    }
}

// --------- Main RTC Time Demo Function ---------
int main(void){

    extern const char* getTime();

    // Initialize HAL Library
    HAL_Init();

    // Configure system clock
    clockConfig();

    // Initialize peripherals
    MX_GPIO_Init();
    MX_USART2_UART_Init();
    MX_RTC_Init();

    // Greeting message
    const char* startMsg = "STM32 RTC Demo Started.\r\n";
    HAL_UART_Transmit(&huart2, (uint8_t*)startMsg, strlen(startMsg), HAL_MAX_DELAY);

    // Main loop
    while (1)
    {
        const char* currentTime = getTime();
        HAL_UART_Transmit(&huart2, (uint8_t*)currentTime, strlen(currentTime), HAL_MAX_DELAY);
        HAL_Delay(1000);
    }
}