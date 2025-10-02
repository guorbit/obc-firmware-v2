#include <Arduino.h>

char receivedChar;
bool newData = false;
int status = EXIT_SUCCESS;

void recvOneChar() {
  if (Serial.available() > 0) {
    receivedChar = Serial.read(); // reads a single character
    newData = true;
  }
}

void exitMode() {
  Serial.println("Press q to exit.");

  while(!newData) {
    recvOneChar();
  }

  switch(receivedChar){
    case 'q':
    case 'Q':
      Serial.println("Exiting...");
      status = EXIT_SUCCESS;
      break;

    default:
      Serial.println("Invalid choice.");
      status = EXIT_FAILURE;
      break;
  }
}

int setTimeMode() {
  Serial.println("Setting time not yet implemented...");
  exitMode();
  return status;
}

int flashDumpMode() {
  Serial.println("Flash dump not yet implemented...");
  exitMode();
  return status;
}

void handleInput() {
  if (newData) {
    newData = false;

    switch (receivedChar){
    case 't':
    case 'T':
      status = setTimeMode();
      Serial.println(status);
      break;

    case 'f':
    case 'F':
      status = flashDumpMode();
      Serial.println(status);
      break;

    default:
      Serial.println("Invalid choice.");
      break;
    }
  }
}

void recovery() {
  Serial.println("Entered recovery mode. \nPress any key to continue.");

  while(!newData) {
    recvOneChar();
  }
  newData = false;

  while (true) {
    Serial.println("Press 't' to set the time, or 'f' to read the flash.");
    newData = false;
    
    while(!newData) {
      recvOneChar();
    }

    handleInput();

  }
}