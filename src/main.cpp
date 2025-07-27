#include "main.h"

// TODO: Add Wathcdog init call

/**
 * @brief The setup function called when the microcontroller retsarts.
 *
 */
void setup()
{

  pinMode(PD13, OUTPUT);
  pinMode(PA0, INPUT_PULLDOWN);
  iwdg::init_watchdog(); // Initialize the watchdog timer
  iwdg::set_key(0x1234);
  // TODO: Set IWDG key to a good value
  // HAL_IWDG_Refresh(&wdog); // Refresh the watchdog timer
}

/**
 * @brief The main loop function that runs repeatedly after setup.
 * @details The function should run all neccary functions nad preform logic in a procedural fomrat
 *
 */
void loop()
{
  // this code loops forever
  while (digitalReadFast(PA_0) == HIGH) // wait for PA0 to be pulled high
  {
    digitalWriteFast(PD_12, HIGH);
  }

  blink(PD_13);
}