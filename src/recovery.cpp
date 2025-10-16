#include <Arduino.h>
#include <flash.h> 

char receivedChar;
bool newData = false; 
int status = EXIT_SUCCESS;

void recvOneChar() {              // receive one character
  if (Serial.available() > 0) {
    receivedChar = Serial.read(); // reads a single character
    newData = true;
  }
}

void exitMode() {                  // exit recovery mode
  Serial.println("Press q to exit.");

  while(!newData) {                // wait for new input
    recvOneChar();
  }

  switch(receivedChar){            // process input
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

int setTimeMode() {                 // stub for setting time
  Serial.println("Setting time not yet implemented...");
  exitMode();
  return status;
}

int flashDumpMode() {               // dump entire flash
  Serial.println("Press y to dump entire flash, any other key to exit.");
  while(!newData) {                 // wait for new input
    recvOneChar();
  }
  switch(receivedChar){
    case 'y':
    case 'Y':
      Serial.println("Dumping entire flash...");
      flashDumpAll();
      status = EXIT_SUCCESS;
      break;

    default:
      Serial.println("Exiting...");
      status = EXIT_FAILURE;
      break;
  }
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

void recovery() {                 // recovery mode
  Serial.println("Entered recovery mode. \nPress any key to continue.");

  while(!newData) {              // wait for new input 
    recvOneChar();
  }
  newData = false;

  while (true) {                // main recovery loop  
    Serial.println("Press 't' to set the time, or 'f' to read the flash.");
    newData = false;

    while(!newData) { 
      recvOneChar();
    }

    handleInput(); 

  }
}