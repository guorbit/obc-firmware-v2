#include "comms.h"
#include "config.h"
#include "LoRa_E32.h"
#include <HardwareSerial.h>
#include <Arduino.h>
#include <Wire.h>


// Temp comms init
HardwareSerial uart0(PA10, PA9);
LoRa_E32 comms(&uart0,
               UART_BPS_RATE_9600); // Config without connect AUX and M0 M1

int sendComms(const char* state){
  // This function is intended to be called every main loop.
  // It maintains its own chunked-send state across calls so that
  // only one 58-byte chunk is sent each second.
  static char pendingMessage[OBC_MESSAGE_LEN] = {};
  static char lastSentMessage[OBC_MESSAGE_LEN] = {};
  static int messageOffset = -1;
  static unsigned long lastChunkSend = 0;
  static unsigned long lastCompleteSend = 0;

  if (state != nullptr && state[0] != '\0') {
    bool stateDiffersFromPending = (strcmp(state, pendingMessage) != 0);
    bool stateDiffersFromLastSent = (strcmp(state, lastSentMessage) != 0);

    if (messageOffset == -1) {
      if (stateDiffersFromPending) {
        strncpy(pendingMessage, state, sizeof(pendingMessage));
        pendingMessage[sizeof(pendingMessage) - 1] = '\0';
        messageOffset = 0;
      } else if (!stateDiffersFromLastSent) {
        if (millis() - lastCompleteSend >= SLOW_LOOP_FREQ) {
          strncpy(pendingMessage, state, sizeof(pendingMessage));
          pendingMessage[sizeof(pendingMessage) - 1] = '\0';
          messageOffset = 0;
        }
      }
    }
  }

  if (messageOffset == -1) {
    return 0;
  }

  if (millis() - lastChunkSend < 1000) {
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
    Serial.print(F("Comms send chunk failed: "));
    Serial.println(status.getResponseDescription());
  } else {
    Serial.print(F("Comms send chunk success ("));
    Serial.print(chunkSize);
    Serial.println(F(" bytes)."));
  }

  messageOffset += chunkSize;
  lastChunkSend = millis();

  if (messageOffset >= totalLen) {
    messageOffset = -1;
    strncpy(lastSentMessage, pendingMessage, sizeof(lastSentMessage));
    lastSentMessage[sizeof(lastSentMessage) - 1] = '\0';
    lastCompleteSend = millis();
    Serial.println(F("Comms message fully sent."));
  }

  return 0;
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

int getComms() {
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

}