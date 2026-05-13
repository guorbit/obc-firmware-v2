#include "comms.h"
#include "LoRa_E32.h"
#include <HardwareSerial.h>
#include <Arduino.h>
#include <Wire.h>


// Temp comms init
HardwareSerial uart0(PA10, PA9);
LoRa_E32 comms(&uart0,
               UART_BPS_RATE_9600); // Config without connect AUX and M0 M1

int sendComms(const char* state){
    
}

int initComms() {
  // Comms CFG pin
  pinMode(PC6, OUTPUT);
  digitalWriteFast(PC_6, LOW);
  delay(100); // Wait for mode switch stabilization

  if (!comms.begin()) {
    Serial.println(F("Comms begin failed!"));
  } else {
    Serial.println(F("Comms begin success."));
  }
}