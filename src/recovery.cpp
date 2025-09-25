#include <Arduino.h>

char receivedChar;
bool newData = false;

void recvOneChar() {
  if (Serial.available() > 0)
  {
    receivedChar = Serial.read(); // reads a single character
    newData = true;
  }
}

void showNewData() {
  if (newData)
  {
    Serial.print("Press 't' to set the time, 'f' to read the flash, or 'q' to quit.");
    newData = false;
  }
}

int setTimeMode() {
  Serial.println("Setting time not yet implemented...");
  return EXIT_SUCCESS;
}

int flashDumpMode() {
  Serial.println("Flash dump not yet implemented...");
  return EXIT_SUCCESS;
}

void recovery() {
  bool running = true;

  while (running) {
    Serial.println("Entered recovery mode. \n Press any key to continue.");

    char choice = receivedChar;
    newData = false;
    int status = EXIT_SUCCESS;

    recvOneChar();
    showNewData();

    switch (choice) {
    case 't':
    case 'T':
      status = setTimeMode();
      break;

    case 'f':
    case 'F':
      status = flashDumpMode();
      break;

    case 'q':
    case 'Q':
      running = false;
      Serial.println("Exiting recovery mode.");
      break;

    default:
      Serial.println("Invalid choice.");
      status = EXIT_FAILURE;
      break;
    }
  }
}