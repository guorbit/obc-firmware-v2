#include <Arduino.h>
#include <blink.h>

void setup()
{
  // this code runs once at startup
  pinMode(PD13, OUTPUT);
}

void loop()
{
  // this code loops forever
  blink(PD_13);
}