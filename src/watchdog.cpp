#include "watchdog.hpp"

// NOTE: err_flag is declared elsewhere; we keep it as a boolean contract here.
extern bool err_flag; // true iff a watchdog-related error occurred
// TODO: Add an err_flag symbol that points to a spot in flash mem that can be read later

// STM32 IWDG timeout formula (from RM):
//   tout = ((RLR + 1) * Prescaler) / f_LSI
// with Prescaler in {4,8,16,32,64,128,256} and f_LSI ~ 32 kHz.

namespace iwdg
{
    // Timing config: choose a pair that really yields ~0.1 s
    constexpr uint32_t LSI_HZ = 32000U; // typical; measure on your board if you need precision

    constexpr uint32_t prescaler = IWDG_PRESCALER_32; // divisor = 32 Medium Power as prescaler means noit much reading and writing
    constexpr uint32_t reload_value = 99U;            // (99 + 1) * 32 / 32000 = 100 ms

    // Handle and local state
    static bool iwdg_is_enabled = false;

    IWDG_HandleTypeDef wdog = {
        .Instance = IWDG,
        .Init = {
            .Prescaler = prescaler,
            .Reload = reload_value}};

    // Default error hook (weak): set the flag; user may override elsewhere.
    void __weak IWDGErrorHandler()
    {
        err_flag = true;
    }

    /**
     * @brief User-friendly watchdog init.
     * Initializes PR/RLR via HAL and locks write access. Clears the error flag.
     */
    void init_watchdog()
    {
        HAL_IWDG_Init(&wdog); // programs prescaler & reload
        __HAL_IWDG_DISABLE_WRITE_ACCESS(&wdog);
    }

    /**
     * @brief Set the reload value (RLR) and kick once.
     * Keeps the legacy name but this programs RLR (not key register).
     */
    void set_reload_key(uint16_t rlr)
    {
        __HAL_IWDG_START(&wdog); // ensure running
        __HAL_IWDG_ENABLE_WRITE_ACCESS(&wdog);
        wdog.Instance->RLR = rlr;         // program reload
        __HAL_IWDG_RELOAD_COUNTER(&wdog); // pet once after updating
        __HAL_IWDG_DISABLE_WRITE_ACCESS(&wdog);

        if (!iwdg_is_enabled)
        {
            iwdg_is_enabled = true;
        }
    }

    /**
     * @brief Init watchdog with BKPSRAM access enabled.
     * Leaves user hook & BKPSRAM plumbing to higher layers.
     */
    void init_watchdog_with_user_error_handling()
    {
        __HAL_RCC_PWR_CLK_ENABLE();

        // If you mirror err_flag into BKPSRAM elsewhere, do it there.
        err_flag = false;

        HAL_IWDG_Init(&wdog);
    }

    /**
     * @brief Pet the watchdog (non-virtual wrapper).
     */
    void pet_watch_dog()
    {
        UNUSED(__pet_watch_dog__());
    }

    /**
     * @brief Weak pet implementation; user may override.
     * @return true on success, false on refresh failure (and raises error flag)
     */
    __weak bool __pet_watch_dog__()
    {
        if (HAL_IWDG_Refresh(&wdog) != HAL_OK)
        {
            err_flag = true;
            IWDGErrorHandler();
            return false;
        }
        return true; // was missing before
    }

    /**
     * @brief Check-and-clear: did the watchdog path report an error?
     * @return true if an error was latched since last call.
     */
    bool has_the_watchdog_barked()
    {
        if (err_flag)
        {
            err_flag = false; // consume once
            return true;
        }
        return false;
    }

} // namespace iwdg
