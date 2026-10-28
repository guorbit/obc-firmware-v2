#include <Arduino.h>
#include <flash.h> 
#include <time.h>
#include "config.h"
#include "recovery.h"
#include "adcs.h" // for automatic time setting
#include "user.h"
#include "credits.h"

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

void readline(char* buffer, size_t maxlen) {
  while (Serial.available() == 0) {} // Wait for input
  size_t len = Serial.readBytesUntil('\n', buffer, maxlen - 1); // Read input until newline, leaving space for null terminator
  buffer[len] = '\0'; // Null-terminate the string  
  // Clear leftover carriage return or extra characters
  char *cr = strchr(buffer, '\r');
  if (cr) *cr = '\0';
  while (Serial.available()) Serial.read();
}

bool validation(const char* input, const char* type) {
  // --- DATE VALIDATION LOGIC ---
  if (strcmp(type, "date") == 0) {
    if (strlen(input) != 10 || input[4] != '-' || input[7] != '-') {
      Serial.println("Invalid date format. Please use YYYY-MM-DD format.");
      return false;
    }

    int month = atoi(&input[5]);
    int day = atoi(&input[8]);

    if (month < 1 || month > 12 || day < 1 || day > 31) {
      Serial.println("Invalid date values. Please ensure month is 1-12 and day is 1-31.");
      return false;
    }
    return true;
  }

  // --- TIME VALIDATION LOGIC ---
  if (strcmp(type, "time") == 0) {
    if (strlen(input) != 8 || input[2] != ':' || input[5] != ':') {
      Serial.println("Invalid time format. Please use HH:MM:SS format.");
      return false;
    }

    int hours = atoi(&input[0]);
    int minutes = atoi(&input[3]);
    int seconds = atoi(&input[6]);

    if (hours < 0 || hours > 23 || minutes < 0 || minutes > 59 || seconds < 0 || seconds > 59) {
      Serial.println("Invalid time values. Please ensure hours are 0-23, minutes and seconds are 0-59.");
      return false;
    }
    return true;
  }

  return false;
}

void timeHourMinSec() {               
  Serial.println("Please enter the current year, month, and day.");
  char Date_Input[11]; // Buffer to hold the date input (10 chars + null terminator)
  readline(Date_Input, sizeof(Date_Input)); // Read the date input using the readline function
  
  // basic format and range validation
  if (!validation(Date_Input, "date")) {
    return;
  }
  
  // extract year, month, and day
  int year = atoi(&Date_Input[0]);
  int month = atoi(&Date_Input[5]);
  int day = atoi(&Date_Input[8]);

  // now get time
  Serial.println("Please enter the current time in HH:MM:SS format.");
  char Time_Input[11]; // Buffer to hold the time input (8 chars + null terminator)
  readline(Time_Input, sizeof(Time_Input)); // Read the time input using the readline function

  // basic format and range validation
  if (!validation(Time_Input, "time")) {
    return;
  }

  // extract hours, minutes, and seconds
  int hours = atoi(&Time_Input[0]); 
  int minutes = atoi(&Time_Input[3]);
  int seconds = atoi(&Time_Input[6]);

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

  // Create struct for time and initialise with defaults
  struct {
    int // all of type int
    year = 2026, 
    month = 3, 
    day = 20, 
    hour = 0, 
    minute = 0, 
    second = 0;
  } gpsTime;

  // Get gps time
  char dataFromADCS[READOUT_LENGTH_ADCS] = "ADCS data not gathered\0";
  initADCS();                  // initialise ADCS
  adcsRead(dataFromADCS);

  if (dataFromADCS[0] == 't') {
    // Extract time from adcs message
    sscanf(dataFromADCS, "t%02d%02d%02d", gpsTime.hour, gpsTime.minute, gpsTime.second);
  } else {
    // Print an error
    Serial.printf("No valid time in message: %s\n", dataFromADCS);
  }

  char gpsTimeChar[32] = {};
  snprintf(gpsTimeChar, sizeof(gpsTimeChar), "%04d%02d%02dT%02d%02d%02dZ", 
          gpsTime.year, gpsTime.month, gpsTime.day, 
          gpsTime.hour, gpsTime.minute, gpsTime.second);


  const char* oldTime = rtcGetTime(); 
  Serial.print("Old rtc time: ");
  Serial.println(oldTime);

  // Set rtc time
  rtcSetTime(gpsTimeChar);

  const char* newTime = rtcGetTime(); 
  Serial.print("New rtc time: ");
  Serial.println(oldTime);

  Serial.println("Time synchronized. Please confirm this time is accurate.");
  Serial.println("Press y to confirm, n to reject. ");

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

int flashEraseMode() {
  Serial.println("Press y to erase entire flash, any other key to exit.");
  Serial.println("You can only erase the flash if you unclick the recovery mode button.");

  while(!newData) {                 // wait for new input
    recvOneChar();
  }

  switch(receivedChar){
    case 'y':
    case 'Y':
      if (checkUser()) {
        break;
      }
      Serial.println("Erasing entire flash...");
      flashEraseAll();
      status = EXIT_SUCCESS;
      break;

    default:
      Serial.println("Exiting...");
      status = EXIT_FAILURE;
      break;
  }
  return status;
}

int creditCalling(){
  credits();
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

    case 'e':
    case 'E': 
      status = flashEraseMode();
      Serial.println(status);
      break;

    case 'c':
    case 'C':
      status=creditCalling();
      Serial.println(status);
      break;

    default:
      Serial.println("Invalid choice.");
      break;
    }
  }
}

void recovery() {                 // recovery mode
  Serial.begin(460800);              // initialize serial for recovery mode

  Serial.setTimeout(60000);   // set timeout to 60 seconds

  while(!newData) {              // wait for new input 
    recvOneChar();
  }
  newData = false;

  Serial.println("Entered recovery mode. \nPress enter to continue.");
  delay(1000);

  while(!newData) {              // wait for new input 
    recvOneChar();
  }
  newData = false;

  while (true) {                // main recovery loop  
    Serial.println("Press 't' to set the time, 'f' to read the flash,'e' to erase the flash, 'c' to print the credits");
    newData = false;

    while(!newData) { 
      recvOneChar();
    }

    handleInput(); 

  }
}

int initRec() {              // initialise recovery mode
  pinMode(PB2, INPUT); // recovery mode pin
  return EXIT_SUCCESS;
}

int checkRec() {             // check if recovery mode should be entered
  if (digitalReadFast(PB_2) == HIGH) {
    recovery();
  } 
  return EXIT_SUCCESS;
}
