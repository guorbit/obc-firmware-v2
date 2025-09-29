#include <Arduino.h>

char receivedChar;
bool newData = false;
bool running = true;
char choice = receivedChar;
int status = EXIT_SUCCESS;

void exitMode() {

  Serial.println("Press q to exit recovery mode");
  switch(choice){
    case 'q':
    case 'Q':
      Serial.println("Exiting recovery mode.");
      running = false;
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
  return EXIT_SUCCESS;
  Serial.println(status);
}

int flashDumpMode() {
  Serial.println("Flash dump not yet implemented...");
  exitMode();
  return EXIT_SUCCESS;
  Serial.println(status);
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
Serial.println("Entered recovery mode. \nPress any key to continue.");

  while (true) {
    Serial.println("Press 't' to set the time, or 'f' to read the flash.");

    char choice = receivedChar;
    newData = false;
    int status = EXIT_SUCCESS;

    while(!newData) {
      recvOneChar();
    }

    handleInput();
  }
}