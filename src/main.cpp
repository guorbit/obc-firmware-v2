#include <Arduino.h>
#include <blink.h>
#include <tmp.h>



void setup()
{
  // this code runs once at startup
   
  // enable status led
  pinMode(PD13, OUTPUT);
   
  // enable serial communication
  Serial.begin(9600);
}

void loop()
{
  // this code loops forever

  // blink status led
  blink(PD_13);

  Serial.printf("TMP: %i\n", tmp());

  delay(50);
}