#include "main.h"

// TTL = (4*800)/32e3
IWDG_HandleTypeDef wdog = {
    .Instance = IWDG,
    .Init = {
        .Prescaler = IWDG_PRESCALER_4,
        .Reload = 800}};

// TODO: Remove once confirmed watchdog is working
void ledStartup()
{
  uint8_t i = 0;
  for (; i < (rand()%45); i++)
  {
    digitalWriteFast((PinName)(PD_12+i%4), HIGH);
    delay(167);
    digitalWriteFast((PinName)(PD_12+(i+3)%4), LOW);
    delay(33);
    
  }

  for (i %=4; i < 4; i++)
  {
    digitalWriteFast((PinName)(PD_12+i), HIGH);
    delay(750);
  }

  for (uint8_t k = 0; k < 6; k++)
  {

    for (i = 0; i < 4; i++)
    {
      digitalWriteFast((PinName)(PD_12+i), LOW);
    }

    if (k>= 5)
      break;

    delay(750);

    for (i = 0; i < 4; i++)
    {
      digitalWriteFast((PinName)(PD_12+i), HIGH);
    }
    delay(750);
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
  if(HAL_IWDG_Refresh(&wdog)!=HAL_OK){
    IWDGErrorHandler(); // if the watchdog refresh fails, call the error handler
  }
  else{
    digitalWriteFast(PD_12, HIGH); // turn on PD_12 to indicate success
    delay(100);
    digitalWriteFast(PD_12, LOW); // turn off PD_12
  }
}