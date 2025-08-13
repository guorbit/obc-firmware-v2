#include "watchdog.hpp"
extern bool err_flag;

// The way to calculate bark time is:
// bark time = (4 * reload value) / prescaler
// RL = 
// For example, if the prescaler is 4 and the reload value is 800,
// TTL = (4*800)/32e3

namespace iwdg
{   static bool iwdg_is_enabled = false;
    constexpr uint_fast32_t bark_time = 1000;             // 1 second
    constexpr uint_fast32_t reload_value = 800;           // reload value for the watchdog
    constexpr uint_fast32_t prescaler = IWDG_PRESCALER_16; // prescaler value for the watchdog

    /**
     * @brief  Watch dog timer handle structure.
     *
     */
    IWDG_HandleTypeDef wdog = {
        .Instance = IWDG,
        .Init = {
            .Prescaler = prescaler,
            .Reload = prescaler}};

    void __weak IWDGErrorHandler()
    {
    }

    /**
     * @brief User friendly watchdog init procedure.
     * @details This function initializes the watchdog timer with a predefined prescaler and reload value.
     *
     */
    void init_watchdog()
    {
        // reset error flag
        HAL_IWDG_Init(&wdog);
    }

    void set_key(){
        __HAL_IWDG_START(&wdog);
    }

    /**
     * @brief Procedure that initializes the watchdog (enables Backup SRAM)
     * @details Also enables sram and uses an source locally scoped variable that can be querried
     * for watch dog barks.
     * @see IWDGErrorHandler
     * @note This function enables watchdog and backup SRAM, it does not start the watchdog timer.
     *       The watchdog timer must be started by calling HAL_IWDG_Start() after this function.
     * @note For user friendly watchdog use init_watchdog;
     *       which allows for user defined error handling.
     * @note This function is weakly defined, so it can be overridden by user defined code if needed.
     */
    void init_watchdog_with_user_error_handling()
    {
        __HAL_RCC_PWR_CLK_ENABLE();
        HAL_PWR_EnableBkUpAccess();
        HAL_PWR_EnableBkUpReg();

        __HAL_RCC_BKPSRAM_CLK_ENABLE();
        // reset error flag
        HAL_IWDG_Init(&wdog);
    }

    /**
     * @brief Procedure that pets the watchdog
     * @details This function refreshes the watchdog timer to prevent it from resetting the system.
     *          It is called periodically in the main loop to ensure the system does not reset.
     * @note This function is weakly defined, so it can be overridden by user defined code if needed.
     */
    void pet_watch_dog()
    {
        UNUSED(__pet_watch_dog__());
    }

    /**
     * @brief Weakly defined function to pet the watchdog.
     * @details This function refreshes the watchdog timer to prevent it from resetting the system.
     *          It is called periodically in the main loop to ensure the system does not reset.
     * @return true if the watchdog was successfully refreshed, false otherwise.
     */
    __weak bool __pet_watch_dog__()
    {
        if (HAL_IWDG_Refresh(&wdog) != HAL_OK)
        {
            err_flag = EIO; // Input/Output error
            IWDGErrorHandler();
            return false;
        }
    }

    /**
     * @brief A user friendly function to check if the watchdog has barked.
     *
     * @return true if watchdog has barked, false otherwise.
     * @details This function checks if the error flag is set, indicating that the watchdog has barked.
     * @note The error flag is reset after checking, so it should be called only once per loop iteration.
     */
    bool has_the_watchdog_barked()
    {
        if (err_flag != 0)
        {
            // reset error flag
            bool ret = err_flag == EIO;
            err_flag = 0;
            return ret;
        }
        return false;
    }
} // namespace iwdg
// End of watchdog.cpp