#include <Arduino.h>
#include <flash.h> 
#include <time.h>
#include "recovery.h"

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
  newData = false;

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

void timeHourMinSec() {               
  Serial.println("Please enter the current year, month, and day.");
  while (Serial.available() == 0) {
    // wait for input
  }
  String dateInput = Serial.readStringUntil('\n');    //read input until enter
  dateInput.trim();                                  //remove any leading/trailing whitespace
  
  // basic format validation
  if (dateInput.length() != 10 || dateInput.charAt(4) != '-' || dateInput.charAt(7) != '-') { 
    Serial.println("Invalid date format. Please use YYYY-MM-DD format.");     
    return;
  }
  
  // extract year, month, and day
  int year = dateInput.substring(0, 4).toInt();
  int month = dateInput.substring(5, 7).toInt();
  int day = dateInput.substring(8, 10).toInt();

  // validate ranges
  if (month < 1 || month > 12 || day < 1 || day > 31) {   
    Serial.println("Invalid date values. Please ensure month is 1-12 and day is 1-31.");
    return;
  }

  // now get time
  Serial.println("Please enter the current time in HH:MM:SS format.");
  while (Serial.available() == 0) {
    // wait for input
  }

  String timeInput = Serial.readStringUntil('\n');    //read input until enter
  timeInput.trim();                                   //remove any leading/trailing whitespace

  // basic format validation
  if (timeInput.length() != 8 || timeInput.charAt(2) != ':' || timeInput.charAt(5) != ':') { 
    Serial.println("Invalid time format. Please use HH:MM:SS format.");     
    return;
  }

  // extract hours, minutes, and seconds
  int hours = timeInput.substring(0, 2).toInt(); 
  int minutes = timeInput.substring(3, 5).toInt();
  int seconds = timeInput.substring(6, 8).toInt();

  // validate ranges
  if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59) {   
    Serial.println("Invalid time values. Please ensure hours are 0-23, minutes and seconds are 0-59.");
    return;
  }

  // print formatted time on one line
  Serial.print("Time set to: ");
  char buffer[25];
  sprintf(buffer, "%04d-%02d-%02dT%02d:%02d:%02dZ", year, month, day, hours, minutes, seconds);
  Serial.println(buffer);
}

int timeSetManual() {                // manual time setting
  Serial.println("Manual time setting selected.");
  timeHourMinSec();
  status = EXIT_SUCCESS;
  return status;
}

int timeSetAuto() {               // automatic time setting
  Serial.println("Retrieving time from GPS...");
  const char* currentTime = rtcGetTime(); 
  Serial.print("Current RTC Time: ");
  Serial.print(currentTime);

  Serial.println("Time synchronized. Please confirm this time is accurate.");
  Serial.println("Press y to confirm, n to reject.");

      while(!newData) {           // wait for new input
      recvOneChar();
      }
      newData = false;

      switch (receivedChar) {
      case 'y':
      case 'Y':
        Serial.println("Time confirmed. Exiting...");
        status = EXIT_FAILURE;
        break;

      case 'n':
      case 'N':
        Serial.println("Time rejected. Please enter manual time setting mode.");
        status = timeSetManual();
        break;
      
      default:
        Serial.println("Invalid choice.");
        status = EXIT_FAILURE;
        break;
      }
  return status;
}

int setTimeMode() {                 // stub for setting time
  Serial.println("Press s to set time, or a for automatic time sync.");

  while(!newData) {                 // wait for new input
    recvOneChar();
  }
  newData = false;

  switch(receivedChar){
    case 's':
    case 'S':
      timeSetManual();                 //manual time set
      status = EXIT_SUCCESS;
      break;

    case 'a':
    case 'A':
      timeSetAuto();                //automatic time set
      status = EXIT_SUCCESS;
      break;

    default:
      Serial.println("Invalid choice.");
      status = EXIT_FAILURE;
      break;
  }
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
  Serial.setTimeout(60000);   // set timeout to 60 seconds

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