#include <Arduino.h>

char receivedChar;
bool newData = false;
bool running = true;
char choice = receivedChar;
int status = EXIT_SUCCESS;

int setTimeMode() {
  Serial.println("Setting time not yet implemented...");
  return EXIT_SUCCESS;
}

int flashDumpMode() {
  Serial.println("Flash dump not yet implemented...");
  return EXIT_SUCCESS;
}

void recvOneChar() {
  if (Serial.available() > 0) {
    receivedChar = Serial.read(); // reads a single character
    newData = true;
  }
}

void handleInput() {
  if (newData) {
    char choice = receivedChar;
    newData = false;
    int status = EXIT_SUCCESS;

    Serial.print("Press 't' to set the time, or 'f' to read the flash.");

    switch (choice) {
    case 't':
    case 'T':
      status = setTimeMode();
      break;

    case 'f':
    case 'F':
      status = flashDumpMode();
      break;

    default:
      Serial.println("Invalid choice.");
      status = EXIT_FAILURE;
      break;
    }
  }
}

void recovery() {

  while (true) {
    Serial.println("Entered recovery mode. \n Press any key to continue.");

    char choice = receivedChar;
    newData = false;
    int status = EXIT_SUCCESS;

    while(!newData) {
      recvOneChar();
    }

    handleInput();
  }
}