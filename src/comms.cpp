#include "comms.h"
#include "config.h"
#include "LoRa_E32.h"
#include <HardwareSerial.h>
#include <Arduino.h>
#include <Wire.h>


// Temp comms init
HardwareSerial uart0(PA_10, PA_9);
LoRa_E32 comms(&uart0,
               UART_BPS_RATE_9600); // Config without connect AUX and M0 M1

int sendComms(const char* message){
  // This function is intended to be called every main loop.
  // It maintains its own chunked-send state across calls so that
  // one 58-byte chunk is sent each second.
  static char pendingMessage[OBC_MESSAGE_LEN] = {};
  static char lastSentMessage[OBC_MESSAGE_LEN] = {};
  static int messageOffset = -1;
  static unsigned long lastChunkSend = 0;
  static unsigned long lastCompleteSend = 0;

  if (message != nullptr && message[0] != '\0') {
    bool messageDiffersFromPending = (strcmp(message, pendingMessage) != 0);
    bool messageDiffersFromLastSent = (strcmp(message, lastSentMessage) != 0);

    if (messageOffset == -1) {
      if (messageDiffersFromPending) {
        strncpy(pendingMessage, message, sizeof(pendingMessage));
        pendingMessage[sizeof(pendingMessage) - 1] = '\0';
        messageOffset = 0;
      } else if (!messageDiffersFromLastSent) {
        if (millis() - lastCompleteSend >= SLOW_LOOP_FREQ) {
          strncpy(pendingMessage, message, sizeof(pendingMessage));
          pendingMessage[sizeof(pendingMessage) - 1] = '\0';
          messageOffset = 0;
        }
      }
    }
  }

  if (messageOffset == -1) {
    return 0;
  }

  if (millis() - lastChunkSend < COMMS_CHUNK_INTERVAL_MILLIS) {
    return 0;
  }

  int totalLen = strlen(pendingMessage);
  int remaining = totalLen - messageOffset;

  if (remaining <= 0) {
    messageOffset = -1;
    return 0;
  }

  int chunkSize = (remaining > 58) ? 58 : remaining;
  char chunk[59];
  strncpy(chunk, pendingMessage + messageOffset, chunkSize);
  chunk[chunkSize] = '\0';

  ResponseStatus status = comms.sendMessage(chunk);
  if (status.code != E32_SUCCESS) {
#if OBC_DEBUG
    Serial.print(F("Comms send chunk failed: "));
    Serial.println(status.getResponseDescription());
#endif
    return -1;
  } 

  messageOffset += chunkSize;
  lastChunkSend = millis();

  if (messageOffset >= totalLen) {
    messageOffset = -1;
    strncpy(lastSentMessage, pendingMessage, sizeof(lastSentMessage));
    lastSentMessage[sizeof(lastSentMessage) - 1] = '\0';
    lastCompleteSend = millis();
  }

  return 0;
}

int initComms() {
  // Comms CFG pin
  pinMode(GPIO_COMMS_CFG, OUTPUT);
  digitalWriteFast(GPIO_COMMS_CFG, LOW);
  delay(100); // Wait for mode switch stabilization

  bool status = comms.begin();

  if (status != E32_SUCCESS) {
    return EXIT_FAILURE;
  }
  return EXIT_SUCCESS;
}

int getComms() {
  digitalWriteFast(GPIO_COMMS_CFG, HIGH);
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

  digitalWriteFast(GPIO_COMMS_CFG, LOW);

  return EXIT_SUCCESS;
}