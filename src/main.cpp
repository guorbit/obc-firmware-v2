// -------------------- Headers --------------------
// General headers
#include <Arduino.h>
#include <Wire.h>
#include "config.h"
#include "blink.h"
// Subsystem headers
#include "recovery.h"
#include "watchdog.hpp"
#include "tmp.h"
#include "heater.h" // heater function support
#include "adcs.h"
#include "eps.h"
#include "flash.h"  // SPI flash support
#include "save.h"
#include "comms.h"
#include "time.h" // RTC support




// Initialise variables
char dataFromADCS[READOUT_LENGTH_ADCS] = "ADCS data not gathered\0";
char obcMessage[OBC_MESSAGE_LEN] = {};
unsigned long lastPrint = 0;
unsigned long lastChunkSend = 0;
int messageOffset = -1; // -1 indicates no message is currently being streamed

void setup() {
// -------------------- Setup --------------------

// Debug mode
#if OBC_DEBUG
  Serial.begin(460800); // initialize serial for debug output
#endif

  // Initialise everything
  initBlink();  // Initialise blinker on status LED
  initFlash();  // initialize SPI flash
  initRTC();    // initialize RTC
  initHeater(); // initialize heater function
  initADCS();   // initialise ADCS
  initEPS();    // initialise EPS
  initComms();  // initialise comms

  pinMode(PB2, INPUT); // recovery mode pin
  if (digitalRead(PB2) == HIGH) {
    recovery(); // enter recovery mode if pin is high
  }

  pinMode(PA0, INPUT); // user button

  pinMode(PE4, OUTPUT); // heater
  pinMode(PE3, OUTPUT); // burnwire
  digitalWriteFast(PE_4, LOW); // turn heater off
  digitalWriteFast(PE_3, LOW); // turn burnwire off

  //#define COMMS_CONFIG_READOUT
  #ifdef COMMS_CONFIG_READOUT

  digitalWriteFast(PC_6, HIGH);
  delay(100); // Wait for mode switch
  ResponseStructContainer commsConfig = comms.getConfiguration();
  if (commsConfig.status.code != E32_SUCCESS) {
    Serial.println(commsConfig.status.getResponseDescription());
  } else {
    Configuration *configuration = (Configuration *)commsConfig.data;
    Serial.println(commsConfig.status.getResponseDescription());

    Serial.println(F("----------------------------------------"));
    Serial.print(F("HEAD : "));  Serial.print(configuration->HEAD, BIN);Serial.print(F(" "));Serial.print(configuration->HEAD, DEC);Serial.print(F(" "));Serial.println(configuration->HEAD, HEX);

    Serial.print(F("AddH : "));  Serial.println(configuration->ADDH, HEX);
    Serial.print(F("AddL : "));  Serial.println(configuration->ADDL, HEX);
    Serial.print(F("Chan : "));  Serial.print(configuration->CHAN, DEC); Serial.print(F(" -> ")); Serial.println(configuration->getChannelDescription());
    Serial.println(F("----------------------------------------"));

    Serial.print(F("SpeedParity      : "));  Serial.println(configuration->SPED.getUARTParityDescription());
    Serial.print(F("SpeedUARTDatte   : "));  Serial.println(configuration->SPED.getUARTBaudRate());
    Serial.print(F("SpeedAirDataRate : "));  Serial.println(configuration->SPED.getAirDataRate());

    Serial.print(F("OptionTrans      : "));  Serial.println(configuration->OPTION.getFixedTransmissionDescription());
    Serial.print(F("OptionPullup     : "));  Serial.println(configuration->OPTION.getIODroveModeDescription());
    Serial.print(F("OptionWakeup     : "));  Serial.println(configuration->OPTION.getWirelessWakeUPTimeDescription());
    Serial.print(F("OptionFEC        : "));  Serial.println(configuration->OPTION.getFECDescription());
    Serial.print(F("OptionPower      : "));  Serial.println(configuration->OPTION.getTransmissionPowerDescription());

    Serial.println(F("----------------------------------------"));
    commsConfig.close();
  }

  digitalWriteFast(PC_6, LOW);
  #endif

  //iwdg::init_watchdog();
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

    // Trigger chunked sending
    messageOffset = 0;
  }

  // Every 1s (Chunked Sending)
  if (messageOffset != -1 && millis() - lastChunkSend >= 1000) {
    iwdg::pet_watch_dog();

    int totalLen = strlen(obcMessage);
    int remaining = totalLen - messageOffset;
    
    if (remaining > 0) {
      int chunkSize = (remaining > 58) ? 58 : remaining;
      char chunk[59];
      strncpy(chunk, obcMessage + messageOffset, chunkSize);
      chunk[chunkSize] = '\0';

      ResponseStatus status = comms.sendMessage(chunk);
      if (status.code != E32_SUCCESS) {
        Serial.print(F("Comms send chunk failed: "));
        Serial.println(status.getResponseDescription());
      } else {
        Serial.print(F("Comms send chunk success ("));
        Serial.print(chunkSize);
        Serial.println(F(" bytes)."));
      }

      messageOffset += chunkSize;
      if (messageOffset >= totalLen) {
        messageOffset = -1; // Finished sending all chunks
        Serial.println(F("Comms message fully sent."));
      }
    } else {
      messageOffset = -1;
    }

    lastChunkSend = millis();
    iwdg::pet_watch_dog();
  }
}
