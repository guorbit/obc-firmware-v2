#ifndef __WATCHDOG_HPP__

#define __WATCHDOG_HPP__

#ifndef HAL_IWDG_MODULE_ENABLED
#define HAL_IWDG_MODULE_ENABLED // Ensure that HAL IWD components are enabled
#endif
#include <stdint.h>
#include <stm32f4xx.h>
#include <stm32f4xx_hal_iwdg.h> // use hal iwdg


//TODO : Add function declarations for watchdog
namespace iwdg
{

    extern IWDG_HandleTypeDef wdog;

    extern void init_watchdog();
    extern void init_watchdog_with_user_error_handling();
    extern void pet_watch_dog();
    extern void set_reload_key(uint16_t key);
} // namespace iwdg

#endif