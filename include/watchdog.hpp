#ifndef __WATCHDOG_HPP__

#define __WATCHDOG_HPP__

#ifndef HAL_IWDG_MODULE_ENABLED
#define HAL_IWDG_MODULE_ENABLED // Ensure that HAL IWD components are enabled
#endif

#include <Arduino.h>
#include <stm32f4xx_hal_iwdg.h> // use hal iwdg
#include <sys/errno.h> // use errno for error handling

//TODO : Add function declarations for watchdog
namespace iwdg
{

    extern IWDG_HandleTypeDef wdog;

    extern void init_watchdog();
    extern void init_watchdog_with_user_error_handling();
    extern void pet_watch_dog();
} // namespace iwdg

#endif