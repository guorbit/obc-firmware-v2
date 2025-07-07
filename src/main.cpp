#include "main.h"
bool errno = false; // used to indicate if an error has occurred
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
  for (; i < 4; i++)
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

  for (uint8_t k = 0; k < 3; k++)
  {

    for (i = 0; i < 4; i++)
    {
      digitalWriteFast((PinName)(PD_12+i), LOW);
    }

    if (k>= 5)
      break;

    delay(250);

    for (i = 0; i < 4; i++)
    {
      digitalWriteFast((PinName)(PD_12+i), HIGH);
    }
    delay(250);
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

  pinMode(PA0, INPUT_PULLDOWN);
  ledStartup();
  errno=false; // reset error flag
  HAL_IWDG_Init(&wdog);
}

void IWDGErrorHandler()
{if(!errno){ // if errno is false, then this is the first time the error handler has been called
    errno = true; // set errno to true to indicate an error has occurred
    digitalWriteFast(PD_14, HIGH); // turn off PD_12 to indicate an error
  }
}

void loop()
{
  // this code loops forever
  while(digitalReadFast(PA_0) == HIGH) // wait for PA0 to be pulled high
  {
    digitalWriteFast(PD_12, HIGH);
  }

  blink(PD_13);
  if(HAL_IWDG_Refresh(&wdog)!=HAL_OK){
    IWDGErrorHandler(); // if the watchdog refresh fails, call the error handler
  }
  else{
    if(currentIntervals==intervalsTotal) // if the blink cycle is complete
    digitalToggleFast(PD_15); // turn on PD_12 to indicate success
  }
}