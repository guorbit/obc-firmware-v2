#include "main.h"
IWDG_HandleTypeDef wdog = {
    .Instance = IWDG,
    .Init = {
        .Prescaler = IWDG_PRESCALER_4,
        .Reload = 80}};

// TODO: Remove once confirmed watchdog is working
void ledStartup()
{
  uint8_t i = 0;
  for (; i < 24; i++)
  {
    digitalWriteFast((PinName)i, HIGH);
    delay(500);
    digitalWriteFast((PinName)i, LOW);
  }

  for (i = 0; i < 4; i++)
  {
    digitalWriteFast((PinName)i, HIGH);
    delay(500);
  }

  for (uint8_t k = 0; k < 6; i++)
  {

    for (i = 0; i < 4; i++)
    {
      digitalWriteFast((PinName)i, LOW);
    }

    if (k == 5)
      break;

    delay(500);

    for (i = 0; i < 4; i++)
    {
      digitalWriteFast((PinName)i, HIGH);
    }
  }
}


// Keep
void setup()
{

  // this code runs once at startup
  pinMode(PD12, OUTPUT);
  pinMode(PD13, OUTPUT);
  pinMode(PD14, OUTPUT);
  pinMode(PD15, OUTPUT);
  ledStartup();
  HAL_IWDG_Init(&wdog);
}

void IWDGErrorHandler()
{
  return;
}

void loop()
{
  // this code loops forever
  blink(PD_13);
  switch (HAL_IWDG_Refresh(&wdog))
  {
  case HAL_OK:
    break;
  default:
    IWDGErrorHandler();
  }
}