// -------------------- Headers --------------------
// General headers
#include "blink.h"
#include "config.h"
#include <Arduino.h>
#include <Wire.h>
// Subsystem headers
#include "adcs.h"
#include "comms.h"
#include "eps.h"
#include "flash.h"  // SPI flash support
#include "heater.h" // heater function support
#include "burnwire.h"
#include "recovery.h"
#include "save.h"
#include "time.h" // RTC support
#include "tmp.h"
#include "watchdog.hpp"
#include "user.h"

// Initialise variables
char dataFromADCS[READOUT_LENGTH_ADCS] = "ADCS data not gathered\0";
char obcMessage[OBC_MESSAGE_LEN] = {};
unsigned long lastPrint = 0;

void setup() {
  // -------------------- Setup --------------------
  delay(8000); // Delay to allow time for connection.

  // Debug mode
  #if OBC_DEBUG
  Serial.begin(460800); // initialize serial for debug output
  #endif

  // Initialise everything
  initBlink();    // Initialise blinker on status LED
  initFlash();    // initialize SPI flash
  initRTC();      // initialize RTC
  initHeater();   // initialize heater function
  initBurnwire(); // initialize burnwire function
  initADCS();     // initialise ADCS
  initEPS();      // initialise EPS
  initComms();    // initialise comms
  initRec();      // initialise recovery mode
  initUser();     // initialise user button

  checkRec();     // check if recovery mode should be entered


  pinMode(PE4, OUTPUT);        // heater
  pinMode(PE3, OUTPUT);        // burnwire
  digitalWriteFast(PE_4, LOW); // turn heater off
  digitalWriteFast(PE_3, LOW); // turn burnwire off

  // iwdg::init_watchdog();
}

void loop() {
  // -------------------- Main Loop --------------------
  // Short delay to slow down loop
  iwdg::pet_watch_dog();
  delay(50);
  iwdg::pet_watch_dog();

  // User button to set heater and burnwire for testing
  if (digitalRead(PA0) == HIGH) {
    // If pressed, set both to HIGH
    digitalWriteFast(PE_4, HIGH);
    digitalWriteFast(PE_3, HIGH);
  } else {
    // Else ensure they are LOW
    digitalWriteFast(PE_4, LOW);
    digitalWriteFast(PE_3, LOW);
  }

// Debug mode
#if OBC_DEBUG
  // Print the temperature every loop
  Serial.printf("TMP: %i\n", tmp());
#endif

  // Blink the status LED to show that the loop is running correctly
  blinkPoll();

  iwdg::pet_watch_dog();

  // Every SLOW_LOOP_FREQ ms (Message Compilation)
  if (millis() - lastPrint >= SLOW_LOOP_FREQ) {
    // Clear OBC message
    obcMessage[0] = '\0';

    iwdg::pet_watch_dog();

    // Collect ADCS data
    adcsRead(dataFromADCS);

    iwdg::pet_watch_dog();

    // Begin buffer with opening square brace
    snprintf(obcMessage, sizeof(obcMessage), "[");

    // Compile message
    iwdg::pet_watch_dog();
    snprintf(obcMessage + strlen(obcMessage),
             sizeof(obcMessage) - strlen(obcMessage), "%s",
             rtcGetTime()); // append time to buffer

    iwdg::pet_watch_dog();
    snprintf(obcMessage + strlen(obcMessage),
             sizeof(obcMessage) - strlen(obcMessage), "|%+02i",
             tmp()); // append TMP value to buffer

    iwdg::pet_watch_dog();
    snprintf(obcMessage + strlen(obcMessage),
             sizeof(obcMessage) - strlen(obcMessage), "|%s",
             dataFromADCS); // append ADCS data to buffer

    iwdg::pet_watch_dog();
    snprintf(obcMessage + strlen(obcMessage),
             sizeof(obcMessage) - strlen(obcMessage), "|%s",
             readEPS()); // append EPS readings to buffer

    // End buffer with closing square brace
    iwdg::pet_watch_dog();
    snprintf(obcMessage + strlen(obcMessage),
             sizeof(obcMessage) - strlen(obcMessage), "]");

    // Save message
    iwdg::pet_watch_dog();
    saveState(obcMessage, strlen(obcMessage));

    // [Debug mode] print message
#if OBC_DEBUG
    Serial.println(obcMessage);
#endif

    // Reset lastPrint
    lastPrint = millis();
  }

  // sendComms is called every loop; it keeps sending the next chunk
  // once per second until the full message is transmitted.
  sendComms(obcMessage);
}
